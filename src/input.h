#pragma once

#include <stdint.h>
#include <unistd.h>

#define ESC 27

typedef enum {
    ARROW_UP,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    NOT_AN_ARROW,
} ArrowKey;

static inline uint8_t read_key(void) {
    uint8_t c;

    if (read(STDIN_FILENO, &c, 1)) {
        return c;
    }

    return 0;
}

ArrowKey read_arrow(uint8_t c);
