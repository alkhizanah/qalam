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

static inline Theme kanagawa_wave(void) {
    return (Theme){
        .editor_background = (Color){0x1f, 0x1f, 0x28},
        .editor_foreground = (Color){0xdc, 0xd7, 0xba},
        .statusline_background = (Color){0x2a, 0x2a, 0x37},
        .statusline_foreground = (Color){0xc8, 0xc0, 0x93},
    };
}
