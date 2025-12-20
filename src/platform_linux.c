#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

#include "app.h"
#include "platform.h"

static FT_Library freetype;

static FT_Face font_face;

bool platform_set_font(const char *font_name) {
    FcPattern *font_pattern = FcNameParse((const FcChar8 *)font_name);

    if (font_pattern == NULL) {
        return false;
    }

    FcConfigSubstitute(NULL, font_pattern, FcMatchPattern);

    FcDefaultSubstitute(font_pattern);

    FcResult result;

    FcPattern *font_matched_pattern = FcFontMatch(NULL, font_pattern, &result);

    if (result != FcResultMatch) {
        FcPatternDestroy(font_pattern);

        return false;
    }

    FcChar8 *font_file_path;

    if (FcPatternGetString(font_matched_pattern, FC_FILE, 0, &font_file_path) !=
        FcResultMatch) {
        FcPatternDestroy(font_matched_pattern);
        FcPatternDestroy(font_pattern);

        return false;
    };

    if (FT_New_Face(freetype, (const char *)font_file_path, 0, &font_face) !=
        0) {
        return false;
    }

    FcPatternDestroy(font_matched_pattern);
    FcPatternDestroy(font_pattern);

    return true;
}

bool platform_set_font_size(size_t font_size) {
    return FT_Set_Pixel_Sizes(font_face, 0, font_size) == 0;
}

bool platform_draw_text(App *app, const char *text, Color fg, size_t x,
                        size_t y) {
    while (*text != '\0') {
        if (FT_Load_Char(font_face, *text++, FT_LOAD_RENDER) != 0) {
            return false;
        }

        FT_GlyphSlot glyph = font_face->glyph;

        for (size_t gy = 0; gy < glyph->bitmap.rows; gy++) {
            for (size_t gx = 0; gx < glyph->bitmap.width; gx++) {
                uint8_t alpha =
                    glyph->bitmap.buffer[gx + gy * glyph->bitmap.width];

                size_t fx = x + gx + glyph->bitmap_left;
                size_t fy = y + gy - glyph->bitmap_top;

                if ((0 < fx && fx < app->width) &&
                    (0 < fy && fy < app->height)) {
                    Color *bg = &app->framebuffer[fx + fy * app->width];

                    Color blend = {
                        .r = (fg.r * alpha + bg->r * (255 - alpha)) / 255,
                        .g = (fg.g * alpha + bg->g * (255 - alpha)) / 255,
                        .b = (fg.b * alpha + bg->b * (255 - alpha)) / 255,
                    };

                    *bg = blend;
                }
            }
        }

        x += glyph->advance.x >> 6;
    }

    return true;
}

int main() {
    if (FT_Init_FreeType(&freetype) != 0) {
        fprintf(stderr, "error: failed to initialize freetype\n");

        return 1;
    }

    if (!FcInit()) {
        fprintf(stderr, "error: failed to initialize fontconfig\n");

        return 1;
    }

    Display *display = XOpenDisplay(NULL);

    if (display == NULL) {
        fprintf(stderr, "error: failed to open x11 display\n");

        return 1;
    }

    Window root = DefaultRootWindow(display);

    if (root == None) {
        fprintf(stderr, "error: no x11 root window found");

        XCloseDisplay(display);

        return 1;
    }

    Window window =
        XCreateSimpleWindow(display, root, 0, 0, 800, 600, 0, 0, 0xffffffff);

    if (window == None) {
        fprintf(stderr, "error: failed to create x11 window");

        XCloseDisplay(display);

        return 1;
    }

    XSelectInput(display, window, StructureNotifyMask);

    XMapWindow(display, window);

    XWindowAttributes wa;

    XGetWindowAttributes(display, window, &wa);

    App app = app_init(wa.width, wa.height);

    uint32_t *backbuffer = malloc(app.width * app.height * sizeof(uint32_t));

    XImage *image = XCreateImage(display, wa.visual, wa.depth, ZPixmap, 0,
                                 (char *)backbuffer, app.width, app.height,
                                 sizeof(Color) * 8, app.width * sizeof(Color));

    GC gc = XCreateGC(display, window, 0, NULL);

    Atom wm_delete_window = XInternAtom(display, "WM_DELETE_WINDOW", False);

    XSetWMProtocols(display, window, &wm_delete_window, 1);

    XEvent event;

    while (true) {
        while (XPending(display)) {
            XNextEvent(display, &event);

            switch (event.type) {
            case ClientMessage:
                if ((Atom)event.xclient.data.l[0] == wm_delete_window) {
                    XDestroyWindow(display, window);
                    XCloseDisplay(display);
                    FcFini();

                    return 0;
                }

                break;
            case ConfigureNotify:
                if (event.xconfigure.width != (int)app.width ||
                    event.xconfigure.height != (int)app.height) {
                    app_resize(&app, event.xconfigure.width,
                               event.xconfigure.height);

                    backbuffer = realloc(backbuffer, app.width * app.height *
                                                         sizeof(uint32_t));

                    XFree(image);

                    image = XCreateImage(display, wa.visual, wa.depth, ZPixmap,
                                         0, (char *)backbuffer, app.width,
                                         app.height, sizeof(Color) * 8,
                                         app.width * sizeof(Color));
                }

                break;
            }
        }

        app_update(&app);

        for (size_t i = 0; i < app.width * app.height; i++) {
            const Color color = app.framebuffer[i];

            backbuffer[i] = color.r << 16 | color.g << 8 | color.b;
        }

        XPutImage(display, window, gc, image, 0, 0, 0, 0, app.width,
                  app.height);
    }
}
