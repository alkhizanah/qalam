#pragma once

#include <stdbool.h>
#include <stddef.h>
#include <string.h>

void enter_raw_mode(void);
void exit_raw_mode(void);

void flush(void);

void append_with_len(const char *cmd, size_t cmd_len);

static inline void append(const char *cmd) {
    append_with_len(cmd, strlen(cmd));
}

static inline void clear_screen(void) { append("\x1b[2J\x1b[H"); }

bool get_cursor_position(int *rows, int *cols);

bool get_window_size(int *rows, int *cols);
