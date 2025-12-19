#include <malloc.h>
#include <stdlib.h>
#include <string.h>

#include "loop.h"

Loop loop_init(size_t width, size_t height) {
    Color *framebuffer = malloc(width * height * sizeof(Color));

    if (framebuffer == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }

    return (Loop){
        .framebuffer = framebuffer,
        .width = width,
        .height = height,
    };
}

void loop_resize(Loop *loop, size_t new_width, size_t new_height) {
    loop->framebuffer =
        realloc(loop->framebuffer, new_width * new_height * sizeof(Color));

    if (loop->framebuffer == NULL) {
        fprintf(stderr, "error: out of memory\n");
        exit(1);
    }

    loop->width = new_width;
    loop->height = new_height;
}

void loop_update(Loop *loop) {
    memset(loop->framebuffer, 0, loop->width * loop->height);
}
