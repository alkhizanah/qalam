#include "image_view.h"

ImageView subimage_of(ImageView image, size_t x, size_t y, size_t width,
                      size_t height) {

    return (ImageView){
        .pixels = image.pixels + x + y * image.stride,
        .width = width,
        .height = height,
        .stride = image.stride,
    };
}
