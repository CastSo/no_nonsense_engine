#include "vector_math.h"

vector3f scale_vec3f(vector3f v, float s) {
    return (vector3f){s * v.x, s * v.y, s * v.z};
}

vector4f scale_vec4f(vector4f v, float s) {
    return (vector4f){s * v.x, s * v.y, s * v.z, s * v.w};
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

vector3f add_vec3f(vector3f a, vector3f b) {
    return (vector3f){a.x + b.x, a.y + b.y, a.z + b.z}; 
}

vector4f add_vec4f(vector4f a, vector4f b) {
    return (vector4f){a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w}; 
}


vector3f subtract_vec3f(vector3f a, vector3f b) {
    return (vector3f){a.x - b.x, a.y - b.y, a.z - b.z}; 
}

vector4f subtract_vec4f(vector4f a, vector4f b) {
    return (vector4f){a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w}; 
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

matrix2x4f multiply_mat4f_mat2x4f(matrix2f a, matrix2x4f b) {
    return (matrix2x4f) {
        a.n00 * b.n00 + a.n01 * b.n10, a.n00 * b.n01 + a.n01 * b.n11, a.n00 * b.n02 + a.n01 * b.n12, a.n00 * b.n03 + a.n01 * b.n13,
        a.n10 * b.n00 + a.n11 * b.n10, a.n10 * b.n01 + a.n11 * b.n11, a.n10 * b.n02 + a.n11 * b.n12, a.n10 * b.n03 + a.n11 * b.n13,
    };
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

matrix4f transpose_mat4f(matrix4f m) {
    return (matrix4f) {m.n00, m.n10, m.n20, m.n30,
                       m.n01, m.n11, m.n21, m.n31,
                       m.n02, m.n12, m.n22, m.n32,
                       m.n03, m.n13, m.n23, m.n33};
}

matrix3f inverse_mat3f(matrix3f m){
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

matrix4f inverse_mat4f(matrix4f m){
    //transpose and inverse
    matrix3f mat3 = {
        m.n00, m.n10, m.n20,
        m.n01, m.n11, m.n21,
        m.n02, m.n12, m.n22
    };

    vector3f t = {m.n03, m.n13, m.n23};

    matrix3f inv_m = inverse_mat3f(mat3);

    vector3f inv_t = scale_vec3f(multiply_mat3f_vec3f(inv_m, t), -1.0f);

    return (matrix4f) {
        inv_m.n00, inv_m.n10, inv_m.n20, inv_t.x, 
        inv_m.n01, inv_m.n11, inv_m.n21, inv_t.y, 
        inv_m.n02, inv_m.n12, inv_m.n22, inv_t.z,
        0,0,0,1
     };

}

matrix2f inverse_mat2f(matrix2f m) {
    double det = 1 / (m.n00 * m.n11 - m.n01 * m.n10);
    
    return (matrix2f) {m.n11/det, -m.n01/det,
                        m.n10/det, -m.n00/det};
}


double determinant(matrix3f m) {
    return (m.n00 * (m.n11 * m.n22 - m.n12 * m.n21) +
            m.n01 * (m.n12 * m.n20 - m.n10 * m.n22) +
            m.n02 * (m.n10 * m.n21 - m.n11 * m.n20) );
            
}

double norm_vec3f(vector3f v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
}

vector3f normalize_vec3f(vector3f v) {
    return (vector3f){v.x/norm_vec3f(v), v.y/norm_vec3f(v), v.z/norm_vec3f(v)};
}

double norm_vec4f(vector4f v) {
    return sqrt(v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w);
}
vector4f normalize_vec4f(vector4f v) {
    return (vector4f){v.x/norm_vec4f(v), v.y/norm_vec4f(v), v.z/norm_vec4f(v), v.w/norm_vec4f(v)};
}

