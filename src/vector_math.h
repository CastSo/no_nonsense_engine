#pragma once
#include "type.h"
#include "math.h"
#define _USE_MATH_DEFINES

vector3f scale_vec3f(vector3f v, float s);
vector4f scale_vec4f(vector4f v, float s);

double dot_vec2f(vector2f a, vector2f b);
double dot_vec3f(vector3f a, vector3f b);
double dot_vec4f(vector4f a, vector4f b);

vector3f add_vec3f(vector3f a, vector3f b);
vector3f subtract_vec3(vector3f a, vector3f b);
vector4f subtract_vec4f(vector4f a, vector4f b);


matrix4f multiply_mat4f(matrix4f a, matrix4f b);
vector3f multiply_mat3f_vec3f(matrix3f m, vector3f v);
vector4f multiply_mat4f_vec4f(matrix4f m, vector4f v);

matrix3f transpose_mat3f(matrix3f m);

double determinant(matrix3f m);
matrix3f inverse(matrix3f m);
vector3f cross(vector3f a, vector3f b);

double norm_vec3f(vector3f v);
vector3f normalize_vec3f(vector3f v);

double norm_vec4f(vector4f v);
vector4f normalize_vec4f(vector4f v);