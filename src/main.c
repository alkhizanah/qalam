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

static int screen_rows;
static int screen_cols;
static struct termios original_termios;

#define BUFFER_MAX 4096

static uint8_t buffer[BUFFER_MAX];
static size_t buffer_len;

void flush(void) {
    write(STDOUT_FILENO, buffer, buffer_len);
    buffer_len = 0;
}

void append_with_len(const char *cmd, size_t cmd_len) {
    for (size_t i = 0; i < cmd_len; i++) {
        buffer[buffer_len++] = cmd[i];

        if (buffer_len >= BUFFER_MAX) {
            flush();
        }
    }
}

void append(const char *cmd) { append_with_len(cmd, strlen(cmd)); }

void enter_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &original_termios);

    struct termios raw = original_termios;
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

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void clear_screen(void) { append("\x1b[2J\x1b[H"); }

void draw_editor_rows(void) {
    for (int y = 0; y < screen_rows; y++) {
        append("~");

        append("\x1b[K");

        if (y < screen_rows - 1) {
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

bool get_window_size(int *rows, int *cols) {
    struct winsize winsize;

    if (ioctl(STDOUT_FILENO, TIOCGWINSZ, &winsize) == -1 ||
        winsize.ws_col == 0) {
        return false;
    } else {
        *rows = winsize.ws_row;
        *cols = winsize.ws_col;
        return true;
    }
}

void handle_sigwinch(int sig) {
    (void)sig;

    if (!get_window_size(&screen_rows, &screen_cols)) {
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

    if (!get_window_size(&screen_rows, &screen_cols)) {
        panic("couldn't get window size");
    }

    signal(SIGWINCH, handle_sigwinch);

    for (;;) {
        refresh_screen();
        process_keys();
    }
}
