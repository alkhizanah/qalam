#pragma once

#include <stddef.h>
#include <stdint.h>

#include "image_view.h"
#include "theme.h"

typedef struct {
    ImageView framebuffer;
    Theme theme;
    size_t line_spacing;
    const char *file_path;
    const char *file_content;
} App;

void app_resize(App *, size_t new_width, size_t new_height);

void app_update(App *);
