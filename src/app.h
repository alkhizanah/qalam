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
    Color *framebuffer;
    size_t width;
    size_t height;
} App;

App app_init(size_t width, size_t height);

void app_resize(App *, size_t new_width, size_t new_height);

void app_update(App *);
