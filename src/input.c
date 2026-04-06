#include <stdint.h>
#include <unistd.h>

#include "input.h"

Key read_key(void) {
    uint8_t c;

    if (read(STDIN_FILENO, &c, 1) != 1) {
        return 0;
    }

    if (c == ESC) {
        char seq[3];

        if (read(STDIN_FILENO, &seq[0], 1) != 1)
            return ESC;

        if (read(STDIN_FILENO, &seq[1], 1) != 1)
            return ESC;

        if (seq[0] == '[') {
            if (seq[1] >= '0' && seq[1] <= '9') {
                if (read(STDIN_FILENO, &seq[2], 1) != 1)
                    return ESC;

                if (seq[2] == '~') {
                    switch (seq[1]) {
                    case '1':
                        return HOME;
                    case '3':
                        return DEL;
                    case '4':
                        return END;
                    case '5':
                        return PAGE_UP;
                    case '6':
                        return PAGE_DOWN;
                    case '7':
                        return HOME;
                    case '8':
                        return END;
                    }
                }
            } else {
                switch (seq[1]) {
                case 'A':
                    return ARROW_UP;
                case 'B':
                    return ARROW_DOWN;
                case 'C':
                    return ARROW_RIGHT;
                case 'D':
                    return ARROW_LEFT;
                case 'H':
                    return HOME;
                case 'F':
                    return END;
                }
            }
        } else if (seq[0] == 'O') {
            switch (seq[1]) {
            case 'H':
                return HOME;
            case 'F':
                return END;
            }
        }
    }

    return c;
}
