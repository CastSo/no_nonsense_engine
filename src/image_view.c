#include "image_view.h"

color4ub *image_view_at(image_view *self, uint32_t x, uint32_t y) {
    return &self->pixels[x + y * self->width];
}
