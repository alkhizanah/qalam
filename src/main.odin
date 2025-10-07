package main

import "core:fmt"
import "core:os"
import "core:strings"
import gl "vendor:OpenGL"

main :: proc() {
    create_window("Qalam", 800, 600)

    init_text()

    load_config()

    load_font_face_from_memory(#load("assets/JetBrainsMonoNerdFontMono-Regular.ttf"))

    set_font_size(24)

    for !window_should_close() {
        begin_drawing()

        clear_background(theme.background)

        y := 24

        for str in strings.split(#load("main.odin"), "\n") {
            draw_text(0, i32(y), str, color_from_hex(0xFFFFFFFF))

            y += 24
        }

        end_drawing()
    }

    destroy_window()
}

