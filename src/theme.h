#pragma once

#include <stddef.h>
#include <stdint.h>

#include "image_view.h"

typedef struct {
    Color editor_background;
    Color editor_foreground;
    Color statusline_background;
    Color statusline_foreground;
} Theme;

Theme kanagawa_wave(void);
