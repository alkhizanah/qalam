package freetype

import "base:runtime"
import cffi "core:c"

when ODIN_OS == .Linux {
    foreign import freetype "system:freetype"
}

@(default_calling_convention = "c", link_prefix = "FT_")
foreign freetype {
    @(require_results)
    Error_String :: proc(error_code : Error) -> cstring ---

    Init_FreeType :: proc(alibrary : ^Library) -> Error ---

    Done_FreeType :: proc(library : Library) -> Error ---

    New_Face :: proc(library : Library, filepathname : cstring, face_index : cffi.long, aface : ^Face) -> Error ---

    New_Memory_Face :: proc(library : Library, file_base : ^Byte, file_size : cffi.long, face_index : cffi.long, aface : ^Face) -> Error ---

    Open_Face :: proc(library : Library, args : ^FT_Open_Args_, face_index : cffi.long, aface : ^Face) -> Error ---

    Attach_File :: proc(face : Face, filepathname : cstring) -> Error ---

    Attach_Stream :: proc(face : Face, parameters : ^FT_Open_Args_) -> Error ---

    Reference_Face :: proc(face : Face) -> Error ---

    Done_Face :: proc(face : Face) -> Error ---

    Select_Size :: proc(face : Face, strike_index : cffi.int) -> Error ---

    Request_Size :: proc(face : Face, req : Size_Request) -> Error ---

    Set_Char_Size :: proc(face : Face, char_width : F26Dot6, char_height : F26Dot6, horz_resolution : cffi.uint, vert_resolution : cffi.uint) -> Error ---

    Set_Pixel_Sizes :: proc(face : Face, pixel_width : cffi.uint, pixel_height : cffi.uint) -> Error ---

    Load_Glyph :: proc(face : Face, glyph_index : cffi.uint, load_flags : cffi.int) -> Error ---

    Load_Char :: proc(face : Face, char_code : cffi.ulong, load_flags : cffi.int) -> Error ---

    Set_Transform :: proc(face : Face, _matrix : ^FT_Matrix_, delta : ^FT_Vector_) ---

    Get_Transform :: proc(face : Face, _matrix : ^FT_Matrix_, delta : ^FT_Vector_) ---

    Render_Glyph :: proc(slot : GlyphSlot, render_mode : Render_Mode) -> Error ---

    Get_Kerning :: proc(face : Face, left_glyph : cffi.uint, right_glyph : cffi.uint, kern_mode : cffi.uint, akerning : ^FT_Vector_) -> Error ---

    Get_Track_Kerning :: proc(face : Face, point_size : Fixed, degree : cffi.int, akerning : ^Fixed) -> Error ---

    Select_Charmap :: proc(face : Face, encoding : Encoding) -> Error ---

    Set_Charmap :: proc(face : Face, charmap : CharMap) -> Error ---

    Get_Charmap_Index :: proc(charmap : CharMap) -> cffi.int ---

    Get_Char_Index :: proc(face : Face, charcode : cffi.ulong) -> cffi.uint ---

    Get_First_Char :: proc(face : Face, agindex : ^cffi.uint) -> cffi.ulong ---

    Get_Next_Char :: proc(face : Face, char_code : cffi.ulong, agindex : ^cffi.uint) -> cffi.ulong ---

    Face_Properties :: proc(face : Face, num_properties : cffi.uint, properties : ^FT_Parameter_) -> Error ---

    Get_Name_Index :: proc(face : Face, glyph_name : cstring) -> cffi.uint ---

    Get_Glyph_Name :: proc(face : Face, glyph_index : cffi.uint, buffer : Pointer, buffer_max : cffi.uint) -> Error ---

    @(require_results)
    Get_Postscript_Name :: proc(face : Face) -> cstring ---

    Get_SubGlyph_Info :: proc(glyph : GlyphSlot, sub_index : cffi.uint, p_index : ^cffi.int, p_flags : ^cffi.uint, p_arg1 : ^cffi.int, p_arg2 : ^cffi.int, p_transform : ^FT_Matrix_) -> Error ---

    Get_FSType_Flags :: proc(face : Face) -> cffi.ushort ---

    Face_GetCharVariantIndex :: proc(face : Face, charcode : cffi.ulong, variantSelector : cffi.ulong) -> cffi.uint ---

    Face_GetCharVariantIsDefault :: proc(face : Face, charcode : cffi.ulong, variantSelector : cffi.ulong) -> cffi.int ---

    @(require_results)
    Face_GetVariantSelectors :: proc(face : Face) -> ^cffi.uint ---

    @(require_results)
    Face_GetVariantsOfChar :: proc(face : Face, charcode : cffi.ulong) -> ^cffi.uint ---

    @(require_results)
    Face_GetCharsOfVariant :: proc(face : Face, variantSelector : cffi.ulong) -> ^cffi.uint ---

    MulDiv :: proc(a : cffi.long, b : cffi.long, c : cffi.long) -> cffi.long ---

    MulFix :: proc(a : cffi.long, b : cffi.long) -> cffi.long ---

    DivFix :: proc(a : cffi.long, b : cffi.long) -> cffi.long ---

    RoundFix :: proc(a : Fixed) -> Fixed ---

    CeilFix :: proc(a : Fixed) -> Fixed ---

    FloorFix :: proc(a : Fixed) -> Fixed ---

    Vector_Transform :: proc(vector : ^FT_Vector_, _matrix : ^FT_Matrix_) ---

    Library_Version :: proc(library : Library, amajor : ^cffi.int, aminor : ^cffi.int, apatch : ^cffi.int) ---

    Face_CheckTrueTypePatents :: proc(face : Face) -> b8 ---

    Face_SetUnpatentedHinting :: proc(face : Face, value : b8) -> b8 ---
}

Mod_Err_Base :: 0
Mod_Err_Autofit :: 0
Mod_Err_BDF :: 0
Mod_Err_Bzip2 :: 0
Mod_Err_Cache :: 0
Mod_Err_CFF :: 0
Mod_Err_CID :: 0
Mod_Err_Gzip :: 0
Mod_Err_LZW :: 0
Mod_Err_OTvalid :: 0
Mod_Err_PCF :: 0
Mod_Err_PFR :: 0
Mod_Err_PSaux :: 0
Mod_Err_PShinter :: 0
Mod_Err_PSnames :: 0
Mod_Err_Raster :: 0
Mod_Err_SFNT :: 0
Mod_Err_Smooth :: 0
Mod_Err_TrueType :: 0
Mod_Err_Type1 :: 0
Mod_Err_Type42 :: 0
Mod_Err_Winfonts :: 0
Mod_Err_GXvalid :: 0
Mod_Err_Sdf :: 0
Mod_Err_Max :: 1
Err_Ok :: 0
Err_Cannot_Open_Resource :: 1
Err_Unknown_File_Format :: 2
Err_Invalid_File_Format :: 3
Err_Invalid_Version :: 4
Err_Lower_Module_Version :: 5
Err_Invalid_Argument :: 6
Err_Unimplemented_Feature :: 7
Err_Invalid_Table :: 8
Err_Invalid_Offset :: 9
Err_Array_Too_Large :: 10
Err_Missing_Module :: 11
Err_Missing_Property :: 12
Err_Invalid_Glyph_Index :: 16
Err_Invalid_Character_Code :: 17
Err_Invalid_Glyph_Format :: 18
Err_Cannot_Render_Glyph :: 19
Err_Invalid_Outline :: 20
Err_Invalid_Composite :: 21
Err_Too_Many_Hints :: 22
Err_Invalid_Pixel_Size :: 23
Err_Invalid_SVG_Document :: 24
Err_Invalid_Handle :: 32
Err_Invalid_Library_Handle :: 33
Err_Invalid_Driver_Handle :: 34
Err_Invalid_Face_Handle :: 35
Err_Invalid_Size_Handle :: 36
Err_Invalid_Slot_Handle :: 37
Err_Invalid_CharMap_Handle :: 38
Err_Invalid_Cache_Handle :: 39
Err_Invalid_Stream_Handle :: 40
Err_Too_Many_Drivers :: 48
Err_Too_Many_Extensions :: 49
Err_Out_Of_Memory :: 64
Err_Unlisted_Object :: 65
Err_Cannot_Open_Stream :: 81
Err_Invalid_Stream_Seek :: 82
Err_Invalid_Stream_Skip :: 83
Err_Invalid_Stream_Read :: 84
Err_Invalid_Stream_Operation :: 85
Err_Invalid_Frame_Operation :: 86
Err_Nested_Frame_Access :: 87
Err_Invalid_Frame_Read :: 88
Err_Raster_Uninitialized :: 96
Err_Raster_Corrupted :: 97
Err_Raster_Overflow :: 98
Err_Raster_Negative_Height :: 99
Err_Too_Many_Caches :: 112
Err_Invalid_Opcode :: 128
Err_Too_Few_Arguments :: 129
Err_Stack_Overflow :: 130
Err_Code_Overflow :: 131
Err_Bad_Argument :: 132
Err_Divide_By_Zero :: 133
Err_Invalid_Reference :: 134
Err_Debug_OpCode :: 135
Err_ENDF_In_Exec_Stream :: 136
Err_Nested_DEFS :: 137
Err_Invalid_CodeRange :: 138
Err_Execution_Too_Long :: 139
Err_Too_Many_Function_Defs :: 140
Err_Too_Many_Instruction_Defs :: 141
Err_Table_Missing :: 142
Err_Horiz_Header_Missing :: 143
Err_Locations_Missing :: 144
Err_Name_Table_Missing :: 145
Err_CMap_Table_Missing :: 146
Err_Hmtx_Table_Missing :: 147
Err_Post_Table_Missing :: 148
Err_Invalid_Horiz_Metrics :: 149
Err_Invalid_CharMap_Format :: 150
Err_Invalid_PPem :: 151
Err_Invalid_Vert_Metrics :: 152
Err_Could_Not_Find_Context :: 153
Err_Invalid_Post_Table_Format :: 154
Err_Invalid_Post_Table :: 155
Err_DEF_In_Glyf_Bytecode :: 156
Err_Missing_Bitmap :: 157
Err_Missing_SVG_Hooks :: 158
Err_Syntax_Error :: 160
Err_Stack_Underflow :: 161
Err_Ignore :: 162
Err_No_Unicode_Glyph_Name :: 163
Err_Glyph_Too_Big :: 164
Err_Missing_Startfont_Field :: 176
Err_Missing_Font_Field :: 177
Err_Missing_Size_Field :: 178
Err_Missing_Fontboundingbox_Field :: 179
Err_Missing_Chars_Field :: 180
Err_Missing_Startchar_Field :: 181
Err_Missing_Encoding_Field :: 182
Err_Missing_Bbx_Field :: 183
Err_Bbx_Too_Big :: 184
Err_Corrupted_Font_Header :: 185
Err_Corrupted_Font_Glyphs :: 186
Err_Max :: 187

/// FT_Fast
Fast :: cffi.int

/// FT_UFast
UFast :: cffi.uint

/// FT_Memory
Memory :: ^FT_MemoryRec_

/// FT_Alloc_Func
Alloc_Func :: proc "c" (memory : Memory, size : cffi.long) -> rawptr

/// FT_Free_Func
Free_Func :: proc "c" (memory : Memory, block : rawptr)

/// FT_Realloc_Func
Realloc_Func :: proc "c" (
    memory : Memory,
    cur_size : cffi.long,
    new_size : cffi.long,
    block : rawptr,
) -> rawptr

/// FT_Stream
Stream :: ^FT_StreamRec_

/// FT_StreamDesc
StreamDesc :: FT_StreamDesc_

/// FT_Stream_IoFunc
Stream_IoFunc :: proc "c" (
    stream : Stream,
    offset : cffi.ulong,
    buffer : ^cffi.uchar,
    count : cffi.ulong,
) -> cffi.ulong

/// FT_Stream_CloseFunc
Stream_CloseFunc :: proc "c" (stream : Stream)

/// FT_StreamRec
StreamRec :: FT_StreamRec_

/// FT_Pos
Pos :: cffi.long

/// FT_Vector
Vector :: FT_Vector_

/// FT_BBox
BBox :: FT_BBox_

/// FT_Bitmap
Bitmap :: FT_Bitmap_

/// FT_Outline
Outline :: FT_Outline_

/// FT_Outline_MoveToFunc
Outline_MoveToFunc :: proc "c" (to : ^FT_Vector_, user : rawptr) -> cffi.int

/// FT_Outline_LineToFunc
Outline_LineToFunc :: proc "c" (to : ^FT_Vector_, user : rawptr) -> cffi.int

/// FT_Outline_ConicToFunc
Outline_ConicToFunc :: proc "c" (
    control : ^FT_Vector_,
    to : ^FT_Vector_,
    user : rawptr,
) -> cffi.int

/// FT_Outline_CubicToFunc
Outline_CubicToFunc :: proc "c" (
    control1 : ^FT_Vector_,
    control2 : ^FT_Vector_,
    to : ^FT_Vector_,
    user : rawptr,
) -> cffi.int

/// FT_Outline_Funcs
Outline_Funcs :: FT_Outline_Funcs_

/// FT_Span
Span :: FT_Span_

/// FT_SpanFunc
SpanFunc :: proc "c" (y : cffi.int, count : cffi.int, spans : ^FT_Span_, user : rawptr)

/// FT_Raster_BitTest_Func
Raster_BitTest_Func :: proc "c" (y : cffi.int, x : cffi.int, user : rawptr) -> cffi.int

/// FT_Raster_BitSet_Func
Raster_BitSet_Func :: proc "c" (y : cffi.int, x : cffi.int, user : rawptr)

/// FT_Raster_Params
Raster_Params :: FT_Raster_Params_

/// FT_Raster
Raster :: ^FT_RasterRec_

/// FT_Raster_NewFunc
Raster_NewFunc :: proc "c" (memory : rawptr, raster : ^Raster) -> cffi.int

/// FT_Raster_DoneFunc
Raster_DoneFunc :: proc "c" (raster : Raster)

/// FT_Raster_ResetFunc
Raster_ResetFunc :: proc "c" (raster : Raster, pool_base : ^cffi.uchar, pool_size : cffi.ulong)

/// FT_Raster_SetModeFunc
Raster_SetModeFunc :: proc "c" (raster : Raster, mode : cffi.ulong, args : rawptr) -> cffi.int

/// FT_Raster_RenderFunc
Raster_RenderFunc :: proc "c" (raster : Raster, params : ^FT_Raster_Params_) -> cffi.int

/// FT_Raster_Funcs
Raster_Funcs :: FT_Raster_Funcs_

/// FT_FWord
FWord :: cffi.short

/// FT_UFWord
UFWord :: cffi.ushort

/// FT_Char
Char :: cffi.schar

/// FT_Byte
Byte :: cffi.uchar

/// FT_Bytes
Bytes :: ^Byte

/// FT_Tag
Tag :: cffi.uint

/// FT_String
String :: cffi.char

/// FT_F2Dot14
F2Dot14 :: cffi.short

/// FT_F26Dot6
F26Dot6 :: cffi.long

/// FT_Fixed
Fixed :: cffi.long

/// FT_Error
Error :: cffi.int

/// FT_Pointer
Pointer :: rawptr

/// FT_Offset
Offset :: cffi.size_t

/// FT_PtrDist
PtrDist :: cffi.ptrdiff_t

/// FT_UnitVector
UnitVector :: FT_UnitVector_

/// FT_Matrix
Matrix :: FT_Matrix_

/// FT_Data
Data :: FT_Data_

/// FT_Generic_Finalizer
Generic_Finalizer :: proc "c" (object : rawptr)

/// FT_Generic
Generic :: FT_Generic_

/// FT_ListNode
ListNode :: ^FT_ListNodeRec_

/// FT_List
List :: ^FT_ListRec_

/// FT_ListNodeRec
ListNodeRec :: FT_ListNodeRec_

/// FT_ListRec
ListRec :: FT_ListRec_

/// FT_Glyph_Metrics
Glyph_Metrics :: FT_Glyph_Metrics_

/// FT_Bitmap_Size
Bitmap_Size :: FT_Bitmap_Size_

/// FT_Library
Library :: ^FT_LibraryRec_

/// FT_Module
Module :: ^FT_ModuleRec_

/// FT_Driver
Driver :: ^FT_DriverRec_

/// FT_Renderer
Renderer :: ^FT_RendererRec_

/// FT_Face
Face :: ^FT_FaceRec_

/// FT_Size
Size :: ^FT_SizeRec_

/// FT_GlyphSlot
GlyphSlot :: ^FT_GlyphSlotRec_

/// FT_CharMap
CharMap :: ^FT_CharMapRec_

/// FT_CharMapRec
CharMapRec :: FT_CharMapRec_

/// FT_Face_Internal
Face_Internal :: ^FT_Face_InternalRec_

/// FT_FaceRec
FaceRec :: FT_FaceRec_

/// FT_Size_Internal
Size_Internal :: ^FT_Size_InternalRec_

/// FT_Size_Metrics
Size_Metrics :: FT_Size_Metrics_

/// FT_SizeRec
SizeRec :: FT_SizeRec_

/// FT_SubGlyph
SubGlyph :: ^FT_SubGlyphRec_

/// FT_Slot_Internal
Slot_Internal :: ^FT_Slot_InternalRec_

/// FT_GlyphSlotRec
GlyphSlotRec :: FT_GlyphSlotRec_

/// FT_Parameter
Parameter :: FT_Parameter_

/// FT_Open_Args
Open_Args :: FT_Open_Args_

/// FT_Size_RequestRec
Size_RequestRec :: FT_Size_RequestRec_

/// FT_Size_Request
Size_Request :: ^FT_Size_RequestRec_

/// FT_Pixel_Mode
Pixel_Mode :: enum cffi.uint {
    NONE  = 0,
    MONO  = 1,
    GRAY  = 2,
    GRAY2 = 3,
    GRAY4 = 4,
    LCD   = 5,
    LCD_V = 6,
    BGRA  = 7,
    MAX   = 8,
}

/// FT_Glyph_Format
Glyph_Format :: enum cffi.uint {
    NONE      = 0,
    COMPOSITE = 1668246896,
    BITMAP    = 1651078259,
    OUTLINE   = 1869968492,
    PLOTTER   = 1886154612,
    SVG       = 1398163232,
}

/// FT_Encoding
Encoding :: enum cffi.uint {
    NONE           = 0,
    MS_SYMBOL      = 1937337698,
    UNICODE        = 1970170211,
    SJIS           = 1936353651,
    PRC            = 1734484000,
    BIG5           = 1651074869,
    WANSUNG        = 2002873971,
    JOHAB          = 1785686113,
    GB2312         = 1734484000,
    MS_SJIS        = 1936353651,
    MS_GB2312      = 1734484000,
    MS_BIG5        = 1651074869,
    MS_WANSUNG     = 2002873971,
    MS_JOHAB       = 1785686113,
    ADOBE_STANDARD = 1094995778,
    ADOBE_EXPERT   = 1094992453,
    ADOBE_CUSTOM   = 1094992451,
    ADOBE_LATIN_1  = 1818326065,
    OLD_LATIN_2    = 1818326066,
    APPLE_ROMAN    = 1634889070,
}

/// FT_Size_Request_Type
Size_Request_Type :: enum cffi.uint {
    NOMINAL  = 0,
    REAL_DIM = 1,
    BBOX     = 2,
    CELL     = 3,
    SCALES   = 4,
    MAX      = 5,
}

/// FT_Render_Mode
Render_Mode :: enum cffi.uint {
    NORMAL = 0,
    LIGHT  = 1,
    MONO   = 2,
    LCD    = 3,
    LCD_V  = 4,
    SDF    = 5,
    MAX    = 6,
}

/// FT_Kerning_Mode
Kerning_Mode :: enum cffi.uint {
    DEFAULT  = 0,
    UNFITTED = 1,
    UNSCALED = 2,
}

/// FT_MemoryRec_
FT_MemoryRec_ :: struct #align (8) {
    user :    rawptr,
    alloc :   Alloc_Func,
    free :    Free_Func,
    realloc : Realloc_Func,
}
#assert(size_of(FT_MemoryRec_) == 32)

/// FT_StreamRec_
FT_StreamRec_ :: struct #align (8) {
    base :       ^cffi.uchar,
    size :       cffi.ulong,
    pos :        cffi.ulong,
    descriptor : FT_StreamDesc_,
    pathname :   FT_StreamDesc_,
    read :       Stream_IoFunc,
    close :      Stream_CloseFunc,
    memory :     Memory,
    cursor :     ^cffi.uchar,
    limit :      ^cffi.uchar,
}
#assert(size_of(FT_StreamRec_) == 80)

/// FT_Vector_
FT_Vector_ :: struct #align (8) {
    x : Pos,
    y : Pos,
}
#assert(size_of(FT_Vector_) == 16)

/// FT_BBox_
FT_BBox_ :: struct #align (8) {
    xMin : Pos,
    yMin : Pos,
    xMax : Pos,
    yMax : Pos,
}
#assert(size_of(FT_BBox_) == 32)

/// FT_Bitmap_
FT_Bitmap_ :: struct #align (8) {
    rows :         cffi.uint,
    width :        cffi.uint,
    pitch :        cffi.int,
    buffer :       ^cffi.uchar,
    num_grays :    cffi.ushort,
    pixel_mode :   cffi.uchar,
    palette_mode : cffi.uchar,
    palette :      rawptr,
}
#assert(size_of(FT_Bitmap_) == 40)

/// FT_Outline_
FT_Outline_ :: struct #align (8) {
    n_contours : cffi.short,
    n_points :   cffi.short,
    points :     ^FT_Vector_,
    tags :       cstring,
    contours :   ^cffi.short,
    flags :      cffi.int,
}
#assert(size_of(FT_Outline_) == 40)

/// FT_Outline_Funcs_
FT_Outline_Funcs_ :: struct #align (8) {
    move_to :  Outline_MoveToFunc,
    line_to :  Outline_LineToFunc,
    conic_to : Outline_ConicToFunc,
    cubic_to : Outline_CubicToFunc,
    shift :    cffi.int,
    delta :    Pos,
}
#assert(size_of(FT_Outline_Funcs_) == 48)

/// FT_Span_
FT_Span_ :: struct #align (2) {
    x :        cffi.short,
    len :      cffi.ushort,
    coverage : cffi.uchar,
}
#assert(size_of(FT_Span_) == 6)

/// FT_Raster_Params_
FT_Raster_Params_ :: struct #align (8) {
    target :      ^FT_Bitmap_,
    source :      rawptr,
    flags :       cffi.int,
    gray_spans :  SpanFunc,
    black_spans : SpanFunc,
    bit_test :    Raster_BitTest_Func,
    _bit_set :    Raster_BitSet_Func,
    user :        rawptr,
    clip_box :    FT_BBox_,
}
#assert(size_of(FT_Raster_Params_) == 96)

/// FT_RasterRec_
FT_RasterRec_ :: struct {}

/// FT_Raster_Funcs_
FT_Raster_Funcs_ :: struct #align (8) {
    glyph_format :    Glyph_Format,
    raster_new :      Raster_NewFunc,
    raster_reset :    Raster_ResetFunc,
    raster_set_mode : Raster_SetModeFunc,
    raster_render :   Raster_RenderFunc,
    raster_done :     Raster_DoneFunc,
}
#assert(size_of(FT_Raster_Funcs_) == 48)

/// FT_UnitVector_
FT_UnitVector_ :: struct #align (2) {
    x : F2Dot14,
    y : F2Dot14,
}
#assert(size_of(FT_UnitVector_) == 4)

/// FT_Matrix_
FT_Matrix_ :: struct #align (8) {
    xx : Fixed,
    xy : Fixed,
    yx : Fixed,
    yy : Fixed,
}
#assert(size_of(FT_Matrix_) == 32)

/// FT_Data_
FT_Data_ :: struct #align (8) {
    pointer : ^Byte,
    length :  cffi.uint,
}
#assert(size_of(FT_Data_) == 16)

/// FT_Generic_
FT_Generic_ :: struct #align (8) {
    data :      rawptr,
    finalizer : Generic_Finalizer,
}
#assert(size_of(FT_Generic_) == 16)

/// FT_ListNodeRec_
FT_ListNodeRec_ :: struct #align (8) {
    prev : ListNode,
    next : ListNode,
    data : rawptr,
}
#assert(size_of(FT_ListNodeRec_) == 24)

/// FT_ListRec_
FT_ListRec_ :: struct #align (8) {
    head : ListNode,
    tail : ListNode,
}
#assert(size_of(FT_ListRec_) == 16)

/// FT_Glyph_Metrics_
FT_Glyph_Metrics_ :: struct #align (8) {
    width :        Pos,
    height :       Pos,
    horiBearingX : Pos,
    horiBearingY : Pos,
    horiAdvance :  Pos,
    vertBearingX : Pos,
    vertBearingY : Pos,
    vertAdvance :  Pos,
}
#assert(size_of(FT_Glyph_Metrics_) == 64)

/// FT_Bitmap_Size_
FT_Bitmap_Size_ :: struct #align (8) {
    height : cffi.short,
    width :  cffi.short,
    size :   Pos,
    x_ppem : Pos,
    y_ppem : Pos,
}
#assert(size_of(FT_Bitmap_Size_) == 32)

/// FT_LibraryRec_
FT_LibraryRec_ :: struct {}

/// FT_ModuleRec_
FT_ModuleRec_ :: struct {}

/// FT_DriverRec_
FT_DriverRec_ :: struct {}

/// FT_RendererRec_
FT_RendererRec_ :: struct {}

/// FT_FaceRec_
FT_FaceRec_ :: struct #align (8) {
    num_faces :           cffi.long,
    face_index :          cffi.long,
    face_flags :          cffi.long,
    style_flags :         cffi.long,
    num_glyphs :          cffi.long,
    family_name :         cstring,
    style_name :          cstring,
    num_fixed_sizes :     cffi.int,
    available_sizes :     ^FT_Bitmap_Size_,
    num_charmaps :        cffi.int,
    charmaps :            ^CharMap,
    generic :             FT_Generic_,
    bbox :                FT_BBox_,
    units_per_EM :        cffi.ushort,
    ascender :            cffi.short,
    descender :           cffi.short,
    height :              cffi.short,
    max_advance_width :   cffi.short,
    max_advance_height :  cffi.short,
    underline_position :  cffi.short,
    underline_thickness : cffi.short,
    glyph :               GlyphSlot,
    size :                Size,
    charmap :             CharMap,
    driver :              Driver,
    memory :              Memory,
    stream :              Stream,
    sizes_list :          FT_ListRec_,
    autohint :            FT_Generic_,
    extensions :          rawptr,
    internal :            Face_Internal,
}
#assert(size_of(FT_FaceRec_) == 248)

/// FT_SizeRec_
FT_SizeRec_ :: struct #align (8) {
    face :     Face,
    generic :  FT_Generic_,
    metrics :  FT_Size_Metrics_,
    internal : Size_Internal,
}
#assert(size_of(FT_SizeRec_) == 88)

/// FT_GlyphSlotRec_
FT_GlyphSlotRec_ :: struct #align (8) {
    library :           Library,
    face :              Face,
    next :              GlyphSlot,
    glyph_index :       cffi.uint,
    generic :           FT_Generic_,
    metrics :           FT_Glyph_Metrics_,
    linearHoriAdvance : Fixed,
    linearVertAdvance : Fixed,
    advance :           FT_Vector_,
    format :            Glyph_Format,
    bitmap :            FT_Bitmap_,
    bitmap_left :       cffi.int,
    bitmap_top :        cffi.int,
    outline :           FT_Outline_,
    num_subglyphs :     cffi.uint,
    subglyphs :         SubGlyph,
    control_data :      rawptr,
    control_len :       cffi.long,
    lsb_delta :         Pos,
    rsb_delta :         Pos,
    other :             rawptr,
    internal :          Slot_Internal,
}
#assert(size_of(FT_GlyphSlotRec_) == 304)

/// FT_CharMapRec_
FT_CharMapRec_ :: struct #align (8) {
    face :        Face,
    encoding :    Encoding,
    platform_id : cffi.ushort,
    encoding_id : cffi.ushort,
}
#assert(size_of(FT_CharMapRec_) == 16)

/// FT_Face_InternalRec_
FT_Face_InternalRec_ :: struct {}

/// FT_Size_InternalRec_
FT_Size_InternalRec_ :: struct {}

/// FT_Size_Metrics_
FT_Size_Metrics_ :: struct #align (8) {
    x_ppem :      cffi.ushort,
    y_ppem :      cffi.ushort,
    x_scale :     Fixed,
    y_scale :     Fixed,
    ascender :    Pos,
    descender :   Pos,
    height :      Pos,
    max_advance : Pos,
}
#assert(size_of(FT_Size_Metrics_) == 56)

/// FT_SubGlyphRec_
FT_SubGlyphRec_ :: struct {}

/// FT_Slot_InternalRec_
FT_Slot_InternalRec_ :: struct {}

/// FT_Parameter_
FT_Parameter_ :: struct #align (8) {
    tag :  cffi.ulong,
    data : Pointer,
}
#assert(size_of(FT_Parameter_) == 16)

/// FT_Open_Args_
FT_Open_Args_ :: struct #align (8) {
    flags :       cffi.uint,
    memory_base : ^Byte,
    memory_size : cffi.long,
    pathname :    cstring,
    stream :      Stream,
    driver :      Module,
    num_params :  cffi.int,
    params :      ^FT_Parameter_,
}
#assert(size_of(FT_Open_Args_) == 64)

/// FT_Size_RequestRec_
FT_Size_RequestRec_ :: struct #align (8) {
    type :           Size_Request_Type,
    width :          cffi.long,
    height :         cffi.long,
    horiResolution : cffi.uint,
    vertResolution : cffi.uint,
}
#assert(size_of(FT_Size_RequestRec_) == 32)

/// FT_StreamDesc_
FT_StreamDesc_ :: struct #raw_union #align (8) {
    value :   cffi.long,
    pointer : rawptr,
}
#assert(size_of(FT_StreamDesc_) == 8)

