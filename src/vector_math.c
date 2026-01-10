#include "vector_math.h"

vector3f scale_vec3(vector3f v, float s) {
    return (vector3f){s * v.x, s * v.y, s * v.z};
}

double dot_vec2f(vector2f a, vector2f b) {
    return a.x * b.x + a.y * b.y;
}

double dot_vec3f(vector3f a, vector3f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

double dot_vec4f(vector4f a, vector4f b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

double norm_vec3f(vector3f v) {
    return sqrt((v.x * v.x) + (v.y * v.y) + (v.z * v.z));
}

vector3f add_vec3(vector3f a, vector3f b) {
    return (vector3f){a.x + b.x, a.y + b.y, a.z + b.z}; 
}

vector3f subtract_vec3(vector3f a, vector3f b) {
    return (vector3f){a.x - b.x, a.y - b.y, a.z - b.z}; 
}


matrix4f multiply_mat4f(matrix4f a, matrix4f b) {
    return (matrix4f){
        a.n00*b.n00 + a.n01*b.n10 + a.n02*b.n20 + a.n03*b.n30,
        a.n00*b.n01 + a.n01*b.n11 + a.n02*b.n21 + a.n03*b.n31,
        a.n00*b.n02 + a.n01*b.n12 + a.n02*b.n22 + a.n03*b.n32,
        a.n00*b.n03 + a.n01*b.n13 + a.n02*b.n23 + a.n03*b.n33,

        a.n10*b.n00 + a.n11*b.n10 + a.n12*b.n20 + a.n13*b.n30,
        a.n10*b.n01 + a.n11*b.n11 + a.n12*b.n21 + a.n13*b.n31,
        a.n10*b.n02 + a.n11*b.n12 + a.n12*b.n22 + a.n13*b.n32,
        a.n10*b.n03 + a.n11*b.n13 + a.n12*b.n23 + a.n13*b.n33,

        a.n20*b.n00 + a.n21*b.n10 + a.n22*b.n20 + a.n23*b.n30,
        a.n20*b.n01 + a.n21*b.n11 + a.n22*b.n21 + a.n23*b.n31,
        a.n20*b.n02 + a.n21*b.n12 + a.n22*b.n22 + a.n23*b.n32,
        a.n20*b.n03 + a.n21*b.n13 + a.n22*b.n23 + a.n23*b.n33,

        a.n30*b.n00 + a.n31*b.n10 + a.n32*b.n20 + a.n33*b.n30,
        a.n30*b.n01 + a.n31*b.n11 + a.n32*b.n21 + a.n33*b.n31,
        a.n30*b.n02 + a.n31*b.n12 + a.n32*b.n22 + a.n33*b.n32,
        a.n30*b.n03 + a.n31*b.n13 + a.n32*b.n23 + a.n33*b.n33
         
    };
    
}


vector3f multiply_mat3f_vec3f(matrix3f m, vector3f v) {
    return (vector3f)
            { m.n00 * v.x + m.n01 * v.y  + m.n02*v.z ,
              m.n10 * v.x + m.n11 * v.y  + m.n12*v.z ,
              m.n20 * v.x + m.n21 * v.y  + m.n22*v.z 
            };
}

vector4f multiply_mat4f_vec4f(matrix4f m, vector4f v) {
    return (vector4f){ 
        m.n00 * v.x + m.n01*v.y + m.n02 * v.z + m.n03 * v.w, 
        m.n10 * v.x + m.n11*v.y + m.n12 * v.z + m.n13 * v.w,
        m.n20 * v.x + m.n21*v.y + m.n22 * v.z + m.n23 * v.w,
        m.n30 * v.x + m.n31*v.y + m.n32 * v.z + m.n33 * v.w };
}


vector3f cross(vector3f a, vector3f b) {
    return (vector3f) {a.y * b.z - a.z * b.y,
                       a.z * b.x - a.x * b.z,
                       a.x * b.y - a.y * b.x};
}

matrix3f transpose_mat3f(matrix3f m) {
    return (matrix3f) {m.n00, m.n10, m.n20,
                       m.n01, m.n11, m.n21,
                       m.n02, m.n12, m.n22};
}

matrix3f inverse(matrix3f m){
    //transpose and inverse
    vector3f a = {m.n00, m.n01, m.n02};
    vector3f b = {m.n10, m.n11, m.n12};
    vector3f c = {m.n20, m.n21, m.n22};

    vector3f r0 = cross(b, c);
    vector3f r1 = cross(c, a);
    vector3f r2 = cross(a, b);

    double invDet = 1.0f / dot_vec3f(r2, c);

    return (matrix3f) {
        r0.x * invDet, r0.y * invDet, r0.z * invDet,
        r1.x * invDet, r1.y * invDet, r1.z * invDet,
        r2.x * invDet, r2.y * invDet, r2.z * invDet,
    };
}

double determinant(matrix3f m) {
    return (m.n00 * (m.n11 * m.n22 - m.n12 * m.n21) +
            m.n01 * (m.n12 * m.n20 - m.n10 * m.n22) +
            m.n02 * (m.n10 * m.n21 - m.n11 * m.n20) );
            
}

double norm(vector3f v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vector3f normalize(vector3f v) {
    return (vector3f){v.x/norm(v), v.y/norm(v), v.z/norm(v)};
}



