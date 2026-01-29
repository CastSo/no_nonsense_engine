#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>

typedef struct vector2f {
    double x, y;
} vector2f;

typedef struct vector3f {
    double x, y, z;
}vector3f;

typedef struct vector4f {
    double x, y, z, w;
}vector4f;



typedef struct matrix2f {
    double n00, n01,
          n10, n11;          
}matrix2f;

typedef struct matrix3f {
    double n00, n01, n02, 
          n10, n11, n12, 
          n20, n21, n22;
}matrix3f;

typedef struct matrix4f {
    double n00, n01, n02, n03,
          n10, n11, n12, n13,
          n20, n21, n22, n23,
          n30, n31, n32, n33;
}matrix4f;

typedef struct matrix2x4f {
    double n00, n01, n02, n03,
           n10, n11, n12, n13;
}matrix2x4f;


inline vector4f as_vector(const vector3f *v) {
    vector4f vector = {v->x, v->y, v->z, 0.0f}; 
    return vector;
};


inline vector4f as_point(const vector3f *v) {
    vector4f point = {v->x, v->y, v->z, 1.0f}; 
    return point;
};


typedef struct color4ub {
    uint8_t r, g, b, a;
} color4ub;

static inline color4ub to_color4ub(const vector4f *c) {
    color4ub result;
    //Color channel ranges from 0.0f to 1.0f;
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


