#pragma once
#include "type.h"
#include "math.h"
#include "vector_math.h"
#define _USE_MATH_DEFINES

typedef struct Model {
    int* triangles;
    struct vector3f* vertices;
    struct vector3f* normals;
    struct vector3f* textures;
    int vertices_size;
    int triangles_size;
    int norm_size;
    int texture_size;
}Model;


typedef struct  Shader Shader;

struct Shader {
    Model *model;
    vector4f color;
    vector4f eye;
    vector4f clip;
    vector4f normal;
    vector4f texture;
    matrix4f ModelView;
    matrix4f Perspective;
    matrix4f Viewport;

};
void pipe_vertex(Shader *shader, int face, int vert, float angle);

void project(vector3f *v, int width, int height);

matrix4f viewport(int x, int y, int w, int h);
matrix4f perspective(double f);
matrix4f lookat(vector3f eye, vector3f center, vector3f up); 

vector4f rotateY(vector4f v, double a);
void rotateX(vector4f *v, double a);
void rotateZ(vector4f *v, double a);
vector4f scale(vector4f v, vector3f s);

vector3f *find_normals(vector3f* v, int vertices_size, int* triangles, int triangles_size);