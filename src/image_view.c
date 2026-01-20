#include "image_view.h"

//Sets the color at particular point in pixel buffer
color4ub *image_view_at(image_view *self, uint32_t x, uint32_t y) {
    if (x >= self->width || y >= self->height) {
        perror("Error coordinates out of bound of surface");
        return 0;
    }
    return &self->pixels[x + y * self->width];
}
