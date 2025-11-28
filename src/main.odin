package main

main :: proc() {
    create_window("Qalam", 800, 600)

    load_config()

    load_text_shaders()

    load_font_face_from_memory(#load("assets/JetBrainsMonoNerdFontMono-Regular.ttf"))

    set_line_height(24)

    // TODO: Implement a file picker instead of opening the source code file
    edit_file(#file)

    for !window_should_close() {
        clear_background(theme.background)

        draw_editor()

        swap_buffers()

        poll_events()
    }

    destroy_window()
}

