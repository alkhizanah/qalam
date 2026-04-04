#ifndef TERM_H
#define TERM_H

#include <malloc.h>
#include <stdint.h>
#include <string.h>

#include <termios.h>
#include <unistd.h>

#ifndef CTRL
#define CTRL(k) ((k) & 0x1f)
#endif

typedef struct {
    struct termios original_termios;
} Term;

Term term_open(void);
uint8_t term_read(void);
void term_close(Term *);

#define TERM_IMPLEMENTATION
#ifdef TERM_IMPLEMENTATION

Term term_open(void) {
    Term term;

    tcgetattr(STDIN_FILENO, &term.original_termios);

    struct termios raw = term.original_termios;
    raw.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    raw.c_oflag &= ~(OPOST);
    raw.c_cflag |= (CS8);
    raw.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    raw.c_cc[VMIN] = 0;
    raw.c_cc[VTIME] = 0;

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);

    const char *cmd = "\x1b[?1049h\x1b[2J\x1b[H";
    write(STDOUT_FILENO, cmd, strlen(cmd));

    return term;
}

uint8_t term_read(void) {
    uint8_t c;

    if (read(STDIN_FILENO, &c, 1)) {
        return c;
    }

    return 0;
}

void term_close(Term *term) {
    const char *cmd = "\x1b[?1049l";

    write(STDOUT_FILENO, cmd, strlen(cmd));

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &term->original_termios);
}

#endif // TERM_IMPLEMENTATION
#endif // TERM_H
