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
} Loop;

Loop loop_init(size_t width, size_t height);

void loop_resize(Loop *, size_t new_width, size_t new_height);

void loop_update(Loop *);
