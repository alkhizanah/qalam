#include <stdarg.h>

#define NOB_IMPLEMENTATION
#include "nob.h"

void pipe_output(Nob_Cmd *src, Nob_Cmd *dst) {
    nob_cmd_run(src, .stdout_path = "command-stdout");

    Nob_String_Builder sb = {0};

    nob_read_entire_file("command-stdout", &sb);

    sb.count--; // remove newline

    nob_delete_file("command-stdout");

    Nob_String_View sv = nob_sb_to_sv(sb);

    Nob_String_View svp = nob_sv_chop_by_delim(&sv, ' ');

    if (svp.count == 0) {
        nob_cmd_append(dst, nob_temp_sv_to_cstr(sv));

        return;
    }

    while (svp.count) {
        nob_cmd_append(dst, nob_temp_sv_to_cstr(svp));

        svp = nob_sv_chop_by_delim(&sv, ' ');
    }
}

int main(int argc, char **argv) {
    NOB_GO_REBUILD_URSELF(argc, argv);

    bool ccls = false;
    bool wayland = false;

    for (int i = 0; i < argc; i++) {
        const char *arg = argv[i];

        if (strcmp(arg, "ccls") == 0) {
            ccls = true;
        } else if (strcmp(arg, "wayland") == 0) {
            wayland = true;
        }
    }

    Nob_Cmd compile = {0};

    nob_cc(&compile);

    nob_cc_flags(&compile);

#ifdef __linux
    Nob_Cmd pkg_config = {0};

    nob_cmd_append(&pkg_config, "pkg-config", "--cflags", "--libs");

    nob_cmd_append(&pkg_config, "freetype2", "fontconfig");

    if (wayland) {
        nob_cmd_append(&pkg_config, "wayland-client");
        nob_cmd_append(&compile, "-DPLATFORM_WAYLAND");
    } else {
        nob_cmd_append(&pkg_config, "x11");
        nob_cmd_append(&compile, "-DPLATFORM_X11");
    }

    pipe_output(&pkg_config, &compile);

    nob_cmd_free(pkg_config);
#endif

    nob_cmd_append(&compile, "-Wno-missing-field-initializers");

    if (ccls) {
        Nob_String_Builder sb = {0};

        for (size_t i = 0; i < compile.count; i++) {
            nob_sb_append_cstr(&sb, compile.items[i]);
            nob_sb_append_cstr(&sb, "\n");
        }

        nob_write_entire_file(".ccls", sb.items, sb.count);
    } else {
        nob_cc_output(&compile, "qalam");
        nob_cc_inputs(&compile, "src/app.c", "src/gfx.c", "src/platform.c");

        if (wayland) {
            nob_cc_inputs(&compile,
                          "src/wayland-protocols/xdg-shell-protocol.c");
            nob_cmd_append(&compile, "-lrt");
        }

        nob_cmd_run(&compile);

        nob_cmd_append(&compile, "./qalam");
        nob_cmd_run(&compile);
    }
}
