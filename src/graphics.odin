package main

import "core:c"
import "core:fmt"
import "core:os"
import gl "vendor:OpenGL"
import "vendor:glfw"

ui : struct {
    width, height : c.int,
    window :        glfw.WindowHandle,
}

Color :: distinct [4]f32

color_from_hex :: proc(hex : u32) -> Color {
    b := hex & 0xff
    g := (hex >> 8) & 0xff
    r := (hex >> 8 >> 8) & 0xff
    a := (hex >> 8 >> 8 >> 8) & 0xff

    return {f32(r) / 255, f32(g) / 255, f32(b) / 255, f32(a) / 255}
}

on_resize :: proc "c" (window : glfw.WindowHandle, width, height : c.int) {
    ui.width, ui.height = width, height

    gl.Viewport(0, 0, ui.width, ui.height)
}

create_window :: proc(title : cstring, desired_width, desired_height : c.int) {
    if !glfw.Init() {
        fmt.println("Could not initialize GLFW, exiting...")

        os.exit(1)
    }

    glfw.WindowHint(glfw.CONTEXT_VERSION_MAJOR, 3)
    glfw.WindowHint(glfw.CONTEXT_VERSION_MINOR, 3)
    glfw.WindowHint(glfw.OPENGL_FORWARD_COMPAT, true)
    glfw.WindowHint(glfw.OPENGL_PROFILE, glfw.OPENGL_CORE_PROFILE)

    ui.window = glfw.CreateWindow(desired_width, desired_height, title, nil, nil)

    if ui.window == nil {
        fmt.println("Could not create window using GLFW, exiting...")

        glfw.Terminate()

        os.exit(1)
    }

    glfw.MakeContextCurrent(ui.window)

    glfw.SwapInterval(1)

    glfw.SetFramebufferSizeCallback(ui.window, on_resize)

    gl.load_up_to(3, 3, glfw.gl_set_proc_address)
}


window_should_close :: proc() -> b32 {
    return glfw.WindowShouldClose(ui.window)
}

destroy_window :: proc() {
    glfw.DestroyWindow(ui.window)

    glfw.Terminate()
}


swap_buffers :: proc () {
    glfw.SwapBuffers(ui.window)
}

poll_events :: glfw.PollEvents

clear_background :: proc(color : Color) {
    gl.ClearColor(color.r, color.g, color.b, color.a)
    gl.Clear(gl.COLOR_BUFFER_BIT)
}

normalize_x :: proc(x : i32) -> f32 {
    x := f32(x) / f32(ui.width)
    x *= 2
    x -= 1
    return x
}

normalize_y :: proc(y : i32) -> f32 {
    y := f32(y) / f32(ui.height)
    y *= 2
    return 1 - y
}

