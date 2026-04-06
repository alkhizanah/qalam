#pragma once

#include <stdint.h>
#include <unistd.h>

enum {
    ESC = 27,
    ARROW_UP = 1000,
    ARROW_DOWN,
    ARROW_LEFT,
    ARROW_RIGHT,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    DEL,
};

typedef int Key;

Key read_key(void);
