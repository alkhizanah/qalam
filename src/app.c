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
}

void app_update(App *app) {
    gfx_draw_rectangle(app->framebuffer, 0, 0, app->framebuffer.width,
                       app->framebuffer.height, app->background);

    const char *text = "Hello, World!";

    size_t text_width, text_height;

    platform_measure_text(text, &text_width, &text_height);

    platform_draw_text(app->framebuffer, text, app->foreground,
                       app->framebuffer.width / 2 - text_width / 2,
                       app->framebuffer.height / 2);
}
