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

bool process_keys_common(Key key) {
    switch (key) {
    case ARROW_DOWN:
        move_cursor(0, +1);
        return true;

    case ARROW_UP:
        move_cursor(0, -1);
        return true;

    case ARROW_LEFT:
        move_cursor(-1, 0);
        return true;

    case ARROW_RIGHT:
        move_cursor(+1, 0);
        return true;

    case PAGE_UP:
    case PAGE_DOWN:
        for (int i = q.screen_rows; i > 0; i--) {
            move_cursor(0, PAGE_UP ? -1 : +1);
        }

        return true;

    case HOME:
        q.cursor_x = 0;
        return true;

    case END:
        q.cursor_x = q.screen_cols - 1;
        return true;

    case ESC:
    case CTRL('c'):
        q.mode = NORMAL_MODE;
        return true;
    }

    return false;
}

void process_keys_in_insert_mode(void) {
    Key key = read_key();

    if (process_keys_common(key))
        return;

    switch (key) {}
}

void process_keys_in_normal_mode(void) {
    Key key = read_key();

    if (process_keys_common(key))
        return;

    switch (key) {
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
    }
}

void process_keys(void) {
    switch (q.mode) {
    case NORMAL_MODE:
        process_keys_in_normal_mode();
        break;

    case INSERT_MODE:
        process_keys_in_insert_mode();
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
