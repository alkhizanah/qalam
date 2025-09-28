package main

main :: proc() {
    create_window("Qalam", 800, 600)

    load_config()

    for !window_should_close() {
        begin_drawing()

        clear_background(theme.background)

        end_drawing()
    }

    destroy_window()
}

