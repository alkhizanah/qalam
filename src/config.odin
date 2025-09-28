package main

theme : struct {
    background, foreground : Color,
}

// TODO: Eventually implement a config file that this should parse, no need for it currently
load_config :: proc() {
    theme = {
        background = color_from_hex(0xFF1A1B26),
        foreground = color_from_hex(0xFFA9B1D6),
    }
}

