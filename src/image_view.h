#pragma once
#include "config.h"
#include "type.h"

typedef struct image_view image_view, *pno;
struct image_view {
    //Points to the draw buffer 
    color4ub *pixels;
    uint32_t width;
    uint32_t height;
    
    color4ub *(*at)(image_view *, uint32_t x, uint32_t y);
    
};

color4ub *image_view_at(image_view *self, uint32_t x, uint32_t y);
