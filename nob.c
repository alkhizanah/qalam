#define NOB_IMPLEMENTATION
#include "nob.h"

static Nob_Cmd cmd = {0};

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    nob_cc(&cmd);
    nob_cc_flags(&cmd);
    nob_cmd_append(&cmd, "-Wno-missing-field-initializers");
    nob_cc_output(&cmd, "qalam");
    nob_cc_inputs(&cmd, "src/loop.c", "src/platform.c");

#ifdef __linux
    nob_cmd_append(&cmd, "-lX11");
#endif

    nob_cmd_run(&cmd);

    nob_cmd_append(&cmd, "./qalam");
    nob_cmd_run(&cmd);
}
