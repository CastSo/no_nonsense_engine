#include "vector_math.h"


float dot_vec2f(vector2f a, vector2f b) {
    return a.x * b.x + a.y * b.y;
}

float dot_vec3f(vector3f a, vector3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

float dot_vec4f(vector4f a, vector4f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

vector3f multiply_mat3f_vec3f(matrix3f m, vector3f v) {
    return (vector3f)
            { v.x * m.n00 + v.y * m.n01 + v.z * m.n02,
              v.x * m.n10 + v.y * m.n11 + v.z * m.n12,
              v.x * m.n20 + v.y * m.n21 + v.z * m.n22
            };
}

