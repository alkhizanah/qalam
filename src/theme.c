#include "theme.h"

Theme kanagawa_wave(void) {
    return (Theme){
        .editor_background = (Color){0x1f, 0x1f, 0x28},
        .editor_foreground = (Color){0xdc, 0xd7, 0xba},
        .statusline_background = (Color){0x2a, 0x2a, 0x37},
        .statusline_foreground = (Color){0xc8, 0xc0, 0x93},
    };
}
