#include <ctype.h>
#include <stdlib.h>

#define TERM_IMPLEMENTATION
#include "term.h"

int main(void) {
    Term term = term_open();

    for (;;) {
        switch (term_read()) {
        case CTRL('q'):
            term_close(&term);
            exit(0);
            break;
        }
    }
}
