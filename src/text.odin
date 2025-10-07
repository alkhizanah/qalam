package main

import "core:c"
import "core:container/priority_queue"
import "core:fmt"
import "core:os"
import gl "vendor:OpenGL"

import "freetype"
import "raqm"

font : struct {
    library : freetype.Library,
    face :    freetype.Face,
    rq :      ^raqm.context_t,
    program : u32,
}

vao, vbo, ebo : u32

init_text :: proc() {
    if freetype.Init_FreeType(&font.library) != 0 {
        fmt.eprintln("Could not initialize freetype")

        os.exit(1)
    }

    gl.GenVertexArrays(1, &vao)
    gl.GenBuffers(1, &vbo)
    gl.GenBuffers(1, &ebo)

    program, program_ok := gl.load_shaders_source(
        #load("shaders/text.vert"),
        #load("shaders/text.frag"),
    )

    if !program_ok {
        fmt.eprintln("Could not compile the text shaders for OpenGL")

        os.exit(1)
    }

    font.program = program
}

load_font_face :: proc(file_path : cstring) {
    if freetype.New_Face(font.library, file_path, 0, &font.face) != 0 {
        fmt.eprintln("Could not open font face")

        os.exit(1)
    }
}

load_font_face_from_memory :: proc(source : string) {
    if freetype.New_Memory_Face(font.library, raw_data(source), i64(len(source)), 0, &font.face) != 0 {
        fmt.eprintln("Could not open font face")

        os.exit(1)
    }
}

set_font_size :: proc(font_size : f32) {
    if freetype.Set_Char_Size(font.face, 0, i64(font_size * 64), 0, 0) != 0 {
        fmt.eprintfln("Could not set font size to %d", font_size)

        os.exit(1)
    }
}

draw_text :: proc(x : i32, y : i32, text : string, color : Color) {
    x, y := x, y

    font.rq = raqm.create()

    if font.rq == nil {
        fmt.println("Could not initialize libraqm")
    }

    defer raqm.destroy(font.rq)

    if !raqm.set_text_utf8(font.rq, raw_data(text), len(text)) {
        fmt.eprintfln("Could not set libraqm's layout text to '%s'", text)

        os.exit(1)
    }

    if !raqm.set_freetype_face(font.rq, font.face) {
        fmt.eprintln("Could not set libraqm's layout font face")

        os.exit(1)
    }

    if !raqm.set_par_direction(font.rq, .DIRECTION_DEFAULT) {
        fmt.eprintln("Could not set libraqm's layout direction to 'default'")

        os.exit(1)
    }

    if !raqm.set_language(font.rq, raw_data(string("en")), 0, 2) {
        fmt.eprintln("Could not set libraqm's layout language to 'en'")

        os.exit(1)
    }

    if !raqm.layout(font.rq) {
        fmt.eprintfln("Could not layout the text '%s'", text)

        os.exit(1)
    }

    glyphs_len : c.size_t
    glyphs_ptr := raqm.get_glyphs(font.rq, &glyphs_len)
    glyphs := glyphs_ptr[:glyphs_len]

    gl.UseProgram(font.program)

    gl.PixelStorei(gl.UNPACK_ALIGNMENT, 1)

    for glyph in glyphs {
        if freetype.Load_Glyph(glyph.ftface, glyph.index, 0) != 0 {
            fmt.eprintln("Could not load a glyph")

            os.exit(1)
        }

        glyph_slot := glyph.ftface.glyph

        if freetype.Render_Glyph(glyph_slot, .NORMAL) != 0 {
            fmt.eprintln("Could not render a glyph")

            os.exit(1)
        }

        x_advance := glyph.x_advance >> 6
        y_advance := glyph.y_advance >> 6
        x_offset := glyph.x_offset >> 6
        y_offset := glyph.y_offset >> 6

        x0 := x + x_offset + glyph_slot.bitmap_left
        y0 := y + y_offset - glyph_slot.bitmap_top
        w := i32(glyph_slot.bitmap.width)
        h := i32(glyph_slot.bitmap.rows)

        vertices : []f32 = {
            // top right (v0)
            normalize_x(x0 + w),
            normalize_y(y0 + h),
            1,
            1,
            // bottom right (v1)
            normalize_x(x0 + w),
            normalize_y(y0),
            1,
            0,
            // bottom left (v2)
            normalize_x(x0),
            normalize_y(y0),
            0,
            0,
            // top left (v3)
            normalize_x(x0),
            normalize_y(y0 + h),
            0,
            1,
        }

        indices : []u32 = {
            // first triangle
            0,
            1,
            3,
            // second trianlge
            1,
            2,
            3,
        }

        gl.BindVertexArray(vao)

        gl.BindBuffer(gl.ARRAY_BUFFER, vbo)

        gl.BufferData(
            gl.ARRAY_BUFFER,
            len(vertices) * size_of(f32),
            raw_data(vertices),
            gl.STATIC_DRAW,
        )

        gl.BindBuffer(gl.ELEMENT_ARRAY_BUFFER, ebo)

        gl.BufferData(
            gl.ELEMENT_ARRAY_BUFFER,
            len(indices) * size_of(u32),
            raw_data(indices),
            gl.STATIC_DRAW,
        )

        gl.VertexAttribPointer(0, 4, gl.FLOAT, false, 4 * size_of(f32), 0)

        gl.EnableVertexAttribArray(0)

        texture : u32

        gl.GenTextures(1, &texture)

        gl.ActiveTexture(gl.TEXTURE0)

        gl.BindTexture(gl.TEXTURE_2D, texture)

        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_S, gl.CLAMP_TO_EDGE)
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_WRAP_T, gl.CLAMP_TO_EDGE)
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR)
        gl.TexParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR)

        gl.TexImage2D(
            gl.TEXTURE_2D,
            0,
            gl.R8,
            i32(glyph_slot.bitmap.width),
            i32(glyph_slot.bitmap.rows),
            0,
            gl.RED,
            gl.UNSIGNED_BYTE,
            glyph_slot.bitmap.buffer,
        )

        gl.Uniform4f(
            gl.GetUniformLocation(font.program, "Color"),
            color.r,
            color.g,
            color.b,
            color.a,
        )

        gl.DrawElements(gl.TRIANGLES, i32(len(indices)), gl.UNSIGNED_INT, nil)

        x += x_advance

        if x >= ui.width {
            break
        }
    }
}

