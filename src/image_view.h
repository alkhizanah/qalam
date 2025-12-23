#pragma once

#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t r;
    uint8_t g;
    uint8_t b;
    uint8_t x; // padding
} Color;

typedef struct {
    Color *pixels;
    size_t width;
    size_t height;
} ImageView;
