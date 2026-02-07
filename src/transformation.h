#pragma once
#include "type.h"
#include "math.h"
#include "vector_math.h"
#define _USE_MATH_DEFINES


typedef struct Camera {
    vector3f position;
    vector3f direction;
    vector3f up;
} Camera;

typedef struct Light {
    vector3f position;
    vector3f direction;
} Light;


typedef struct  Shader {
    Camera *camera;
    Light *light;
    vector4f color;
    matrix4f ModelView;
    matrix4f Perspective;
    matrix4f Viewport;

    //Triangles in different spaces
    vector4f clip[3];
    vector3f varying_uv[3];
    vector3f ndc[3];
    vector4f norm[3];

} Shader;


void project(vector3f *v, int width, int height);

matrix4f viewport(int x, int y, int w, int h);
matrix4f perspective(float f);
matrix4f lookat(vector3f eye, vector3f center, vector3f up); 

vector4f rotateY(vector4f v, float a);
vector4f rotateX(vector4f v, float a);
vector4f rotateZ(vector4f v, float a);
vector4f scale(vector4f v, vector3f s);
vector4f translate(vector4f v, vector3f t);

vector3f *find_normals(vector3f* v, int vertices_size, int* triangles, int triangles_size);