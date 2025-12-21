#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "gfx.h"
#include "platform.h"

App app_init(size_t width, size_t height) {
    Color *framebuffer = malloc(width * height * sizeof(Color));

    if (framebuffer == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }

    if (!platform_set_font("monospace")) {
        fprintf(stderr, "error: could not set font to monospace\n");
        exit(1);
    }

    if (!platform_set_font_size(40)) {
        fprintf(stderr, "error: could not set font size to 40\n");
        exit(1);
    }

    return (App){
        .framebuffer = framebuffer,
        .width = width,
        .height = height,
    };
}

void app_resize(App *app, size_t new_width, size_t new_height) {
    app->framebuffer =
        realloc(app->framebuffer, new_width * new_height * sizeof(Color));

    if (app->framebuffer == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }

    app->width = new_width;
    app->height = new_height;
}

void app_update(App *app) {
    memset(app->framebuffer, 0, app->width * app->height * sizeof(Color));

    gfx_draw_rectangle(app, 0, 0, 40 * 8, 50, (Color){0, 0, 255});

    platform_draw_text(app, "Hello, World!", (Color){255, 0, 0},
                       10, 40);

}
