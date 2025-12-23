#pragma once

#include <stddef.h>

#include "image_view.h"

void gfx_draw_rectangle(ImageView, size_t sx, size_t sy, size_t width,
                        size_t height, Color);
