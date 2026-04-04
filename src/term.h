#ifndef TERM_H
#define TERM_H


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

#endif // TERM_IMPLEMENTATION
#endif // TERM_H
