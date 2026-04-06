#pragma once

#include <stddef.h>
#include <stdint.h>
#include <termios.h>

#define BUFFER_MAX 4096

typedef enum {
    NORMAL_MODE,
    INSERT_MODE,
} QalamMode;

typedef struct {
    QalamMode mode;

    int cursor_x;
    int cursor_y;

    int screen_rows;
    int screen_cols;

    uint8_t buffer[BUFFER_MAX];
    size_t buffer_len;

    struct termios original_termios;
} Qalam;

// Provided by main.c only once, this is the global state where all modules
// communicate through
extern Qalam q;
