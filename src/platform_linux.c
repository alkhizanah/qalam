#include <errno.h>
#include <fcntl.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdio.h>
#include <sys/mman.h>
#include <time.h>
#include <unistd.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <fontconfig/fontconfig.h>

#include "app.h"
#include "platform.h"

#if defined(PLATFORM_X11)
#include "platform_linux_x11.c"
#elif defined(PLATFORM_WAYLAND)
#include "platform_linux_wayland.c"
#endif

static FT_Library freetype;

static FT_Face font_face;

bool platform_set_font(const char *font_family) {
    FcPattern *font_pattern = FcNameParse((const FcChar8 *)font_family);

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

bool platform_draw_text(ImageView image, const char *text, Color fg, size_t x,
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

                if ((0 < fx && fx < image.width) &&
                    (0 < fy && fy < image.height)) {
                    Color *bg = image.pixels + fx + fy * image.width;

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

    return platform_main_loop();
}
