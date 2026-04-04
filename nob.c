#include <stdarg.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    Nob_Cmd cmd = {0};

    nob_cc(&cmd);

    nob_cc_flags(&cmd);

    nob_cmd_append(&cmd, "-Wno-missing-field-initializers");

    nob_cc_output(&cmd, "qalam");
    nob_cc_inputs(&cmd, "src/main.c", "src/theme.c");

    nob_cmd_run(&cmd);

    nob_cmd_append(&cmd, "./qalam");
    
    for (int i = 1; i < argc; i++) {
        nob_cmd_append(&cmd, argv[i]);
    }

    nob_cmd_run(&cmd);
}
