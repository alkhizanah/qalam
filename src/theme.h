#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t r, g, b, a;
} Color;

typedef struct {
    Color editor_background;
    Color editor_foreground;
    Color statusline_background;
    Color statusline_foreground;
} Theme;

Theme kanagawa_wave(void);
