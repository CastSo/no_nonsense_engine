#pragma once
#include "type.h"
#include "math.h"
#define _USE_MATH_DEFINES

float dot_vec2f(vector2f a, vector2f b);
float dot_vec3f(vector3f a, vector3f b);
float dot_vec4f(vector4f a, vector4f b);

vector3f multiply_mat3f_vec3f(matrix3f m, vector3f v);

