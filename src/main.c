#include <assert.h>
#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>

#include "input.h"
#include "qalam.h"
#include "terminal.h"

Qalam q;

void panic(const char *reason) {
    clear_screen();
    flush();
    exit_raw_mode();
    printf("%s\n", reason);
    exit(1);
}

void move_cursor(int x_off, int y_off) {
    int x_new = q.cursor_x + x_off;
    int y_new = q.cursor_y + y_off;

    if (x_new >= 0 && x_new < q.screen_cols) {
        q.cursor_x = x_new;
    }

    if (y_new >= 0 && y_new < q.screen_rows) {
        q.cursor_y = y_new;
    }
}

void draw_cursor(void) {
    char buf[32];
    snprintf(buf, sizeof(buf), "\x1b[%d;%dH", q.cursor_y + 1, q.cursor_x + 1);
    append_with_len(buf, strlen(buf));
}

void draw_editor_rows(void) {
    for (int y = 0; y < q.screen_rows; y++) {
        append("~");

        append("\x1b[K");

        if (y < q.screen_rows - 1) {
            append("\r\n");
        }
    }
}

void draw_frame(void) {
    append("\x1b[?25l");
    append("\x1b[H");
    draw_editor_rows();
    draw_cursor();
    append("\x1b[?25h");
    flush();
}

void process_keys_in_insert_mode(uint8_t c) {
    switch (c) {
    case ESC:
    case CTRL('c'):
        q.mode = NORMAL_MODE;
        break;
    }
}

void process_keys_in_normal_mode(uint8_t c) {
    switch (c) {
    case 'j':
        move_cursor(0, +1);
        break;

    case 'k':
        move_cursor(0, -1);
        break;

    case 'h':
        move_cursor(-1, 0);
        break;

    case 'l':
        move_cursor(+1, 0);
        break;

    case 'i':
        q.mode = INSERT_MODE;
        break;

    case CTRL('q'):
        clear_screen();
        flush();
        exit_raw_mode();
        exit(0);
        break;
    }
}

void process_keys(void) {
    uint8_t c = read_key();

    switch (read_arrow(c)) {
    case ARROW_UP:
        move_cursor(0, -1);
        c = read_key();
        break;

    case ARROW_DOWN:
        move_cursor(0, +1);
        c = read_key();
        break;

    case ARROW_LEFT:
        move_cursor(-1, 0);
        c = read_key();
        break;

    case ARROW_RIGHT:
        move_cursor(+1, 0);
        c = read_key();
        break;

    case NOT_AN_ARROW:
    default:
        break;
    }

    switch (q.mode) {
    case NORMAL_MODE:
        process_keys_in_normal_mode(c);
        break;

    case INSERT_MODE:
        process_keys_in_insert_mode(c);
        break;

    default:
        assert(false && "UNREACHABLE");
        break;
    }
}

void process_window_resize_signal(int sig) {
    (void)sig;

    if (!get_window_size(&q.screen_rows, &q.screen_cols)) {
        panic("couldn't get window size");
    }
}

int main(void) {
    if (!get_window_size(&q.screen_rows, &q.screen_cols)) {
        panic("couldn't get window size");
    }

    signal(SIGWINCH, process_window_resize_signal);

    enter_raw_mode();

    q.mode = NORMAL_MODE;
    q.cursor_x = 0;
    q.cursor_y = 0;

    for (;;) {
        draw_frame();
        process_keys();
    }
}
