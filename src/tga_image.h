#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include "image_view.h"

#pragma pack(push,1)
typedef struct TGAHeader {
    uint8_t  id_length ;
    uint8_t  colormap_type ;
    uint8_t  data_type_code ;
    uint16_t colormap_origin ;
    uint16_t colormap_length ;
    uint8_t  colormap_depth ;
    uint16_t x_origin ;
    uint16_t y_origin ;
    uint16_t width ;
    uint16_t height ;
    uint8_t  bits_per_pixel ;
    uint8_t  image_descriptor ;
}TGAHeader;
#pragma pack(pop)

typedef struct TGAColor {
    uint8_t bgra[4];
    uint8_t bytesapp;
}TGAColor;

typedef struct TGAImage {
    uint8_t *data;
    int data_size;
    int w, h;
    uint8_t bpp;
} TGAImage;

int fget_little_short(FILE *f);

color4ub* load_tga(char *file_name, TGAHeader *tga_header);
void flip_horizontally(color4ub *pixbuf, int width, int height);
void flip_vertically(color4ub *pixbuf, int width, int height);