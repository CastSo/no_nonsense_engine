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
    vector4f color;

    float angle;
    float scale;
}Model;

typedef struct Camera {
    vector3f position;
    vector3f direction;
    vector3f up;
} Camera;

typedef struct Light {
    vector3f position;
    vector3f direction;
} Light;


typedef struct  Shader Shader;

struct Shader {
    Camera *camera;
    Light *light;
    vector4f color;
    vector4f eye;
    vector4f clip;
    vector4f normal;
    vector4f texture;
    matrix4f ModelView;
    matrix4f Perspective;
    matrix4f Viewport;

};
void pipe_vertex(Shader *shader, Model *model, int face, int vert, float move);

void project(vector3f *v, int width, int height);

matrix4f viewport(int x, int y, int w, int h);
matrix4f perspective(double f);
matrix4f lookat(vector3f eye, vector3f center, vector3f up); 

vector4f rotateY(vector4f v, double a);
vector4f rotateX(vector4f v, double a);
vector4f rotateZ(vector4f v, double a);
vector4f scale(vector4f v, vector3f s);
vector4f translate(vector4f v, vector3f t);

vector3f *find_normals(vector3f* v, int vertices_size, int* triangles, int triangles_size);