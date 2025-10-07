package main

import "core:fmt"
import "core:os"
import "core:strings"

editor : struct {
    file_path : string,
    lines :     []string,
}

edit_file :: proc(file_path : string) -> (ok : bool) {
    text, err := os.read_entire_file_or_err(file_path)
    ok = err == nil
    editor.lines, err = strings.split_lines(string(text))
    ok = err == nil
    editor.file_path = file_path
    return
}


draw_editor :: proc() {
    y := font.line_height

    for line in editor.lines {
        draw_text(0, y, line, color_from_hex(0xFFFFFFFF))

        y += font.line_height
    }
}

