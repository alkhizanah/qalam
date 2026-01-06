#pragma once

#include <stddef.h>
#include <stdint.h>

#include "image_view.h"

typedef struct {
    Color background;
    Color foreground;
    size_t font_size;
} Theme;
