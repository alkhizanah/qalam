#include <ctype.h>
#include <malloc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <signal.h>
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>

#define BUFFER_MAX 4096

typedef struct {
    struct termios original_termios;

    int screen_rows;
    int screen_cols;

    uint8_t buffer[BUFFER_MAX];
    size_t buffer_len;
} Qalam;

static Qalam q;

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

void append(const char *cmd) { append_with_len(cmd, strlen(cmd)); }

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

uint8_t read_key(void) {
    uint8_t c;

    if (read(STDIN_FILENO, &c, 1)) {
        return c;
    }

    return 0;
}

void exit_raw_mode(void) {
    append("\x1b[?1049l");

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &q.original_termios);
}

void clear_screen(void) { append("\x1b[2J\x1b[H"); }

void draw_editor_rows(void) {
    for (int y = 0; y < q.screen_rows; y++) {
        append("~");

        append("\x1b[K");

        if (y < q.screen_rows - 1) {
            append("\r\n");
        }
    }
}

void refresh_screen(void) {
    append("\x1b[?25l");
    append("\x1b[H");
    draw_editor_rows();
    append("\x1b[H");
    append("\x1b[?25h");
    flush();
}

void panic(const char *reason) {
    clear_screen();
    flush();
    exit_raw_mode();
    printf("%s\n", reason);
    exit(1);
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

void handle_sigwinch(int sig) {
    (void)sig;

    if (!get_window_size(&q.screen_rows, &q.screen_cols)) {
        panic("couldn't get window size");
    }
}

void process_keys(void) {
    switch (read_key()) {
    case CTRL('q'):
        clear_screen();
        flush();
        exit_raw_mode();
        exit(0);
        break;
    }
}

int main(void) {
    enter_raw_mode();

    if (!get_window_size(&q.screen_rows, &q.screen_cols)) {
        panic("couldn't get window size");
    }

    signal(SIGWINCH, handle_sigwinch);

    for (;;) {
        refresh_screen();
        process_keys();
    }
}
