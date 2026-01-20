#pragma once
#include "image_view.h"
#include "vector_math.h"
#include "type.h"
#include "tga_image.h"

typedef struct Model {
    TGAHeader *tga_header;
    int* triangles;
    struct color4ub* uv;
    struct vector3f* vertices;
    struct vector3f* normals;
    struct vector3f* textures;
    int vertices_size;
    int triangles_size;
    int norm_size;
    int texture_size;
    vector4f color;

    float angle;
    float scale;
}Model;


struct Model* read_model_lines(char *file_name);
vector4f normal(Model self, vector2f uv);