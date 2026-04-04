#include <ctype.h>
#include <malloc.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>

void write_into_screen(const char *cmd) {
    write(STDOUT_FILENO, cmd, strlen(cmd));
}

static struct termios original_termios;

void enter_raw_mode(void) {
    tcgetattr(STDIN_FILENO, &original_termios);

    struct termios raw = original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    write_into_screen("\x1b[?1049h\x1b[2J\x1b[H");
}

uint8_t read_key(void) {
    uint8_t c;

    if (read(STDIN_FILENO, &c, 1)) {
        return c;
    }

    return 0;
}

void exit_raw_mode(void) {
    write_into_screen("\x1b[?1049l");

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &original_termios);
}

void clear_screen(void) { write_into_screen("\x1b[2J\x1b[H"); }

void draw_into_screen(void) {
    // Do nothing for now
}

void process_keys(void) {
    switch (read_key()) {
    case CTRL('q'):
        clear_screen();
        exit_raw_mode();
        exit(0);
        break;
    }
}

int main(void) {
    enter_raw_mode();

    for (;;) {
        clear_screen();
        draw_into_screen();
        process_keys();
    }
}
