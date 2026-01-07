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

static size_t statusline_height;

void app_draw_editor(App *app) {
    ImageView editor_image =
        subimage_of(app->framebuffer, 0, 0, app->framebuffer.width,
                    app->framebuffer.height - statusline_height);

    gfx_clear(editor_image, app->theme.editor_background);

    if (app->file_content != NULL) {
        const char *b = app->file_content;

        size_t line = 1;
        size_t column = 0;

        while (*b != '\0') {
            const char c = *b++;

            if (c == '\n') {
                line++;
                column = 0;
            } else {
                const char t[] = {c, 0};

                size_t tw, th;

                platform_measure_text(t, &tw, &th);

                platform_draw_text(editor_image, t,
                                   app->theme.editor_foreground, column * tw,
                                   line * (app->font_size + app->line_spacing));
                column++;
            }
        }
    }
}

void app_draw_statusline(App *app) {
    ImageView statusline_image = subimage_of(
        app->framebuffer, 0, app->framebuffer.height - statusline_height,
        app->framebuffer.width, statusline_height);

    gfx_clear(statusline_image, app->theme.statusline_background);

    if (app->file_path == NULL) {
        platform_draw_text(statusline_image, "[No Name]",
                           app->theme.statusline_foreground, 0, app->font_size);
    } else {
        platform_draw_text(statusline_image, app->file_path,
                           app->theme.statusline_foreground, 0, app->font_size);
    }
}

void app_update(App *app) {
    statusline_height = app->font_size + 5;
    app_draw_editor(app);
    app_draw_statusline(app);
}
