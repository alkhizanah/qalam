#include <stdbool.h>
#include <stdio.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#include "qalam.h"
#include "terminal.h"

void enter_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &q.original_termios);

    struct termios raw = q.original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 1;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    append("\x1b[?1049h\x1b[2J\x1b[H");
}

void exit_raw_mode(void) {
    append("\x1b[?1049l");

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &q.original_termios);
}

void flush(void) {
    write(STDOUT_FILENO, q.buffer, q.buffer_len);
    q.buffer_len = 0;
}

void append_with_len(const char *cmd, size_t cmd_len) {
    for (size_t i = 0; i < cmd_len; i++) {
        q.buffer[q.buffer_len++] = cmd[i];

        if (q.buffer_len >= BUFFER_MAX) {
            flush();
        }
    }
}

bool get_cursor_position(int *rows, int *cols) {
    char buf[32];

    unsigned int i = 0;

    if (write(STDOUT_FILENO, "\x1b[6n", 4) != 4)
        return false;

    while (i < sizeof(buf) - 1) {
        if (read(STDIN_FILENO, &buf[i], 1) != 1)
            break;

        if (buf[i] == 'R')
            break;

        i++;
    }

    buf[i] = '\0';

    if (buf[0] != '\x1b' || buf[1] != '[')
        return false;

    if (sscanf(&buf[2], "%d;%d", rows, cols) != 2)
        return false;

    return true;
}

bool get_window_size(int *rows, int *cols) {
    struct winsize winsize;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize) == -1 ||
        winsize.ws_col == 0) {
        if (write(STDOUT_FILENO, "\x1b[999C\x1b[999B", 12) != 12)
            return false;

        return get_cursor_position(rows, cols);
    } else {
        *rows = winsize.ws_row;
        *cols = winsize.ws_col;
        return true;
    }
}
