#include "transformation.h"

void project(vector3f *v, int width, int height) {
    *v = (vector3f) { (v->x + 1.f) * width/2,
                        (1.f - v->y ) * height/2,
                        (v->z + 1.f) * 255/2
                        };
}

void perspective(vector3f *v) {
    double c = 12.;
    *v = (vector3f){v->x/(1-v->z/c), v->y/(1-v->z/c), v->z/(1-v->z/c)};
}

void rotation(vector3f *v, double a) {
    
    matrix3f mat = (matrix3f){cos(a), 0, sin(a),
                                0,  1,  0,
                              -sin(a), 0, cos(a)};
    *v = multiply_mat3f_vec3f(mat, *v);
}