#pragma once
#include "type.h"
#include "math.h"
#define _USE_MATH_DEFINES

vector3f scale_vec3f(vector3f v, float s);
vector4f scale_vec4f(vector4f v, float s);

float dot_vec2f(vector2f a, vector2f b);
float dot_vec3f(vector3f a, vector3f b);
float dot_vec4f(vector4f a, vector4f b);

vector3f cross_vec3f(vector3f a, vector3f b);
vector4f cross_vec4f(vector4f a, vector4f b);

vector3f add_vec3f(vector3f a, vector3f b);
vector4f add_vec4f(vector4f a, vector4f b);
vector3f subtract_vec3f(vector3f a, vector3f b);
vector4f subtract_vec4f(vector4f a, vector4f b);
vector3f multiply_vec3f(vector3f a, vector3f b);
vector4f multiply_vec4f(vector4f a, vector4f b);

vector2i subtract_vec2i(vector2i a, vector2i b);


matrix4f multiply_mat4f(matrix4f a, matrix4f b);
vector3f multiply_mat3f_vec3f(matrix3f m, vector3f v);
vector4f multiply_mat4f_vec4f(matrix4f m, vector4f v);
matrix2x4f multiply_mat4f_mat2x4f(matrix2f a, matrix2x4f b);

matrix3f transpose_mat3f(matrix3f m);
matrix4f transpose_mat4f(matrix4f m);


float determinant(matrix3f m);
matrix3f inverse_mat3f(matrix3f m);
matrix4f inverse_mat4f(matrix4f m);
matrix2f inverse_mat2f(matrix2f m);
vector3f cross(vector3f a, vector3f b);

float norm_vec3f(vector3f v);
vector3f normalize_vec3f(vector3f v);

float norm_vec4f(vector4f v);
vector4f normalize_vec4f(vector4f v);