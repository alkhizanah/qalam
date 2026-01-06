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

static size_t statusline_height = 25;

void app_draw_editor(App *app) {
    ImageView editor_image =
        subimage_of(app->framebuffer, 0, 0, app->framebuffer.width,
                    app->framebuffer.height - statusline_height);

    gfx_clear(editor_image, app->theme.editor_background);
}

void app_draw_statusline(App *app) {
    ImageView statusline_image = subimage_of(
        app->framebuffer, 0, app->framebuffer.height - statusline_height,
        app->framebuffer.width, statusline_height);

    gfx_clear(statusline_image, app->theme.statusline_background);

    platform_draw_text(statusline_image, "Hello, World!", app->theme.statusline_foreground, 0, app->font_size);
}

void app_update(App *app) {
    app_draw_editor(app);
    app_draw_statusline(app);
}
