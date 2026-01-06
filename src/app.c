#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "app.h"
#include "gfx.h"
#include "platform.h"

void app_resize(App *app, size_t new_width, size_t new_height) {
    app->framebuffer.pixels = realloc(app->framebuffer.pixels,
                                      new_width * new_height * sizeof(Color));

    if (app->framebuffer.pixels == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }

    app->framebuffer.width = new_width;
    app->framebuffer.height = new_height;
    app->framebuffer.stride = new_width;
}

void app_update(App *app) {
    gfx_clear(app->framebuffer, app->theme.background);

    size_t gap = 20;

    ImageView image = {
        .pixels = app->framebuffer.pixels + gap * (app->framebuffer.width + 1),
        .width = app->framebuffer.width - gap * 2,
        .height = app->framebuffer.height - gap * 2,
        .stride = app->framebuffer.width,
    };

    gfx_clear(image, (Color){.r = 60, .g = 70, .b = 80});
}
