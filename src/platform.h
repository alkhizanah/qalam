#pragma once

#include <stdbool.h>

#include "app.h"

bool platform_set_font(const char *font_name);
bool platform_set_font_size(size_t);
bool platform_draw_text(App *, const char *, Color, size_t x, size_t y);
