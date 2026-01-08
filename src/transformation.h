#pragma once
#include "type.h"
#include "math.h"
#include "vector_math.h"
#define _USE_MATH_DEFINES

typedef struct Model {
    struct vector3f* vertices;
    int* triangles;
    int vertices_size;
    int triangles_size;
}Model;


typedef struct  Shader Shader;

struct Shader {
    Model *model;
    vector4f color;
    vector3f tri_eye[3];
    vector4f vertex;
    matrix4f ModelView;
    matrix4f Perspective;
    matrix4f Viewport;

};
void pipe_vertex(Shader *shader,  int face, int vert);

void project(vector3f *v, int width, int height);

matrix4f viewport(int x, int y, int w, int h);
matrix4f perspective(double f);
matrix4f lookat(vector3f eye, vector3f center, vector3f up); 

void rotation(vector3f *v, double a);

