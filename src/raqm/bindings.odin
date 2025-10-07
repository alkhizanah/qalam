package raqm

import "core:c"

import "../freetype"

when ODIN_OS == .Linux {
    foreign import raqm "system:raqm"
}

set_text :: #force_inline proc "contextless" (rq : ^context_t, text : string) {
    set_text_utf8(rq, raw_data(text), len(text))
}

@(default_calling_convention = "c", link_prefix = "raqm_")
foreign raqm {
    create :: proc() -> ^context_t ---
    destroy :: proc(rq : ^context_t) ---

    clear_contents :: proc(rq : ^context_t) ---

    set_text_utf8 :: proc(rq : ^context_t, text : [^]c.char, len : c.size_t) -> bool ---
    set_text_utf16 :: proc(rq : ^context_t, text : [^]u16, len : c.size_t) -> bool ---
    set_text_utf32 :: proc(rq : ^context_t, text : [^]rune, len : c.size_t) -> bool ---

    set_par_direction :: proc(rq : ^context_t, dir : direction_t) -> bool ---

    set_language :: proc(rq : ^context_t, lang : [^]c.char, start : c.size_t, len : c.size_t) -> bool ---

    add_font_feature :: proc(rq : ^context_t, feature : [^]c.char, len : c.int) -> bool ---

    set_freetype_face :: proc(rq : ^context_t, face : freetype.Face) -> bool ---

    set_freetype_face_range :: proc(rq : ^context_t, face : freetype.Face, start : c.size_t, len : c.size_t) -> bool ---

    set_freetype_load_flags :: proc(rq : ^context_t, flags : c.int) -> bool ---

    set_freetype_load_flags_range :: proc(rq : ^context_t, flags : c.int, start : c.size_t, len : c.size_t) -> bool ---

    set_letter_spacing_range :: proc(rq : ^context_t, spacing : c.int, start : c.size_t, len : c.size_t) -> bool ---

    set_word_spacing_range :: proc(rq : ^context_t, spacing : c.int, start : c.size_t, len : c.size_t) -> bool ---

    set_invisible_glyph :: proc(rq : ^context_t, gid : c.int) -> bool ---

    layout :: proc(rq : ^context_t) -> bool ---

    get_glyphs :: proc(rq : ^context_t, length : ^c.size_t) -> [^]glyph_t ---

    get_par_resolved_direction :: proc(rq : ^context_t) -> direction_t ---

    get_direction_at_index :: proc(rq : ^context_t, index : c.size_t) -> direction_t ---

    index_to_position :: proc(rq : ^context_t, index : ^c.size_t, x : ^c.int, y : ^c.int) -> bool ---

    position_to_index :: proc(rq : ^context_t, x : c.int, y : c.int, index : ^c.size_t) -> bool ---

    version :: proc(major : ^c.uint, minor : ^c.uint, micro : ^c.uint) ---

    version_string :: proc() -> cstring ---

    version_atleast :: proc(major : c.uint, minor : c.uint, micro : c.uint) -> bool ---

}

context_t :: struct {}

direction_t :: enum c.int {
    DIRECTION_DEFAULT,
    DIRECTION_RTL,
    DIRECTION_LTR,
    DIRECTION_TTB,
}

glyph_t :: struct {
    index :     c.uint,
    x_advance : c.int,
    y_advance : c.int,
    x_offset :  c.int,
    y_offset :  c.int,
    cluster :   i32,
    ftface :    freetype.Face,
}

