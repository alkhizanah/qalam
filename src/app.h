#pragma once

#include <stddef.h>
#include <stdint.h>

#include "image_view.h"

typedef struct {
    ImageView framebuffer;
    size_t font_size;
    Color background;
    Color foreground;
} App;

void app_resize(App *, size_t new_width, size_t new_height);

void app_update(App *);
