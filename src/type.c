#include "type.h"

vector4f subtract_vec4f(const vector4f *v0, const vector4f *v1) {
    vector4f v0Subtractv1 = {v0->x - v1->x, v0->y - v1->y, v0->z - v1->z, v0->w - v1->w};
    return v0Subtractv1;
};

float determinant(const vector4f *v0, const vector4f *v1) {
    return v0->x * v1->y - v0->y * v1->x;
};






