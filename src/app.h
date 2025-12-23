#pragma once

#include <stddef.h>
#include <stdint.h>

#include "image_view.h"

typedef struct {
    ImageView framebuffer;
    const char *font_family;
    size_t text_size;
    Color background;
    Color foreground;
} App;

App app_init(size_t width, size_t height, const char *font_family,
             size_t text_size, Color background, Color foreground);

void app_resize(App *, size_t new_width, size_t new_height);

void app_update(App *);
