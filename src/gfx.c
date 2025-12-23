#include "gfx.h"

void gfx_draw_rectangle(ImageView image, size_t sx, size_t sy, size_t width, size_t height,
                        Color color) {
    for (size_t y = sy; y < sy + height; y++) {
        for (size_t x = sx; x < sx + width; x++) {
            if ((0 < x && x < image.width) && (0 < y && y < image.height)) {
                image.pixels[x + y * image.width] = color;
            }
        }
    }
}
