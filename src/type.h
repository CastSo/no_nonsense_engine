#pragma once
#include "config.h"



typedef struct vector3f {
    float x, y, z;
}vector3f;

typedef struct vector4f {
    float x, y, z, w;
}vector4f;

inline vector4f as_vector(const vector3f *v) {
    vector4f vector = {v->x, v->y, v->z, 0.0f}; 
    return vector;
};

inline vector4f as_point(const vector3f *v) {
    vector4f point = {v->x, v->y, v->z, 1.0f}; 
    return point;
};

vector4f subtract_vec4f(const vector4f *v0, const vector4f *v1);

float determinant(const vector4f *v0, const vector4f *v1);

typedef struct color4ub {
    uint8_t r, g, b, a;
} color4ub;

static inline color4ub to_color4ub(const vector4f *c) {
    color4ub result;
    //Color channel ranges from 0.0f = 1.0f;
    result.r = fmax(0.f, fmin(255.f, c->x * 255.f));
    result.g = fmax(0.f, fmin(255.f, c->y * 255.f));
    result.b = fmax(0.f, fmin(255.f, c->z * 255.f));
    result.a = fmax(0.f, fmin(255.f, c->w * 255.f));

    return result;
}

typedef struct mesh {
    vector3f *positions;
    uint32_t vertex_count;
    vector4f color;
} mesh;

typedef struct draw_command {
    struct mesh mesh;
} draw_command;


