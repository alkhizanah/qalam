#pragma once

#include <stdbool.h>

#include "image_view.h"

bool platform_set_font(const char *font_name);
bool platform_set_font_size(size_t);
bool platform_draw_text(ImageView, const char *, Color, size_t x, size_t y);
