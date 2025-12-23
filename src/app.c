#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "gfx.h"
#include "platform.h"

App app_init(size_t width, size_t height, const char *font_name,
             size_t text_size, Color background, Color foreground) {
    Color *framebuffer = malloc(width * height * sizeof(Color));

    if (framebuffer == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }

    if (!platform_set_font(font_name)) {
        fprintf(stderr, "error: could not set font to %s\n", font_name);
        exit(1);
    }

    if (!platform_set_font_size(text_size)) {
        fprintf(stderr, "error: could not set font size to %zu\n", text_size);
        exit(1);
    }

    return (App){
        .framebuffer =
            {
                .pixels = framebuffer,
                .width = width,
                .height = height,
            },
        .font_name = font_name,
        .text_size = text_size,
        .background = background,
        .foreground = foreground,
    };
}

void app_resize(App *app, size_t new_width, size_t new_height) {
    app->framebuffer.pixels = realloc(app->framebuffer.pixels,
                                      new_width * new_height * sizeof(Color));

    if (app->framebuffer.pixels == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }

    app->framebuffer.width = new_width;
    app->framebuffer.height = new_height;
}

void app_update(App *app) {
    gfx_draw_rectangle(app->framebuffer, 0, 0, app->framebuffer.width,
                       app->framebuffer.height, app->background);

    platform_draw_text(app->framebuffer, "Hello, World!", app->foreground,
                       app->framebuffer.width / 2 - app->text_size * 4,
                       app->framebuffer.height / 2);
}
