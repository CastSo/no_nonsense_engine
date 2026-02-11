#pragma once
#include "image_view.h"
#include "vector_math.h"
#include "type.h"
#include "tga_image.h"
#include "util/util.h"

typedef struct Model {
    TGAHeader header_uv;
    TGAHeader header_diffuse;
    TGAHeader header_specular;

    int* triangles;
    //Texture images
    struct color4ub* uv;
    struct color4ub* diffuse;
    struct color4ub* specular;

    //Obj data
    struct vector3f* vertices;
    struct vector3f* normals;
    struct vector3f* textures;
    uint8_t *texturebuf;
    int vertices_size;
    int triangles_size;
    int norm_size;
    int texture_size;
    vector4f color;

    vector3f position;
    float angle;
    float scale;
}Model;

typedef struct Sprite {
    int* triangles;
    struct vector3f* vertices;
    struct vector3f* textures;

    int vertices_size;
    int textures_size;
    int triangles_size;
}sprite;


struct Model load_obj(char *file_name);
vector4f normal(TGAHeader tga_header, color4ub *image, vector2f uv);
color4ub sample2D(TGAHeader tga_header, color4ub *image, vector2f uv);