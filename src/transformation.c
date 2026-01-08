#include "transformation.h"


 void pipe_vertex(Shader *shader,  int face, int vert) {
    vector3f v = shader->model->vertices[shader->model->triangles[face+vert]-1];
    vector4f position = multiply_mat4f_vec4f(shader->ModelView, (vector4f){v.x, v.y, v.z, 1.}); // in object coordinates
    shader->tri_eye[face] = (vector3f){position.x, position.y, position.z}; //in eye coordinates
    shader->vertex = multiply_mat4f_vec4f(shader->Perspective, position); // in clip coordinates
}

void project(vector3f *v, int width, int height) {
    *v = (vector3f) { (v->x + 1.f) * width/2,
                        (1.f - v->y ) * height/2,
                        (v->z + 1.f) * 255/2
                        };
}

matrix4f viewport(int x, int y, int w, int h) {
    return (matrix4f){
        w/2., 0, 0, x+w/2., 
        0, h/2., 0, y+h/2., 
        0, 0, 1, 0, 
        0, 0, 0, 1};
}

matrix4f perspective(double f) {
    return (matrix4f){
        1, 0, 0, 0, 
        0, 1, 0, 0, 
        0, 0, 1, 0, 
        0, 0, -1/f, 1};
}

matrix4f lookat(vector3f eye, vector3f center, vector3f up) {
    vector3f n = normalize(subtract_vec3(eye, center));
    vector3f l = normalize(cross(up, n));
    vector3f m = normalize(cross(n, (vector3f){1}));
    matrix4f mat1 = {l.x,l.y,l.z,0, m.x,m.y,m.z,0, n.x,n.y,n.z,0, 0,0,0,1};
    matrix4f mat2 = {1,0,0,-center.x, 0,1,0,-center.y, 0,0,1,-center.z, 0,0,0,1};
    return multiply_mat4f(mat1, mat2);

}

void rotation(vector3f *v, double a) {
    
    matrix3f mat = (matrix3f){cos(a), 0, sin(a),
                                0,  1,  0,
                              -sin(a), 0, cos(a)};
    *v = multiply_mat3f_vec3f(mat, *v);
}