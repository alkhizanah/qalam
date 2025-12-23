#pragma once

#include <stdbool.h>

#include "image_view.h"

bool platform_set_font(const char *);
bool platform_set_font_size(size_t);
bool platform_draw_text(ImageView, const char *, Color, size_t x, size_t y);
bool platform_measure_text(const char *, size_t *width, size_t *height);
