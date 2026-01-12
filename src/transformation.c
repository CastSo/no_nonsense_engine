#include "transformation.h"


 void pipe_vertex(Shader *shader, int face, int vert, float angle) {
    vector3f scaling = (vector3f) {0.5, 0.5, 0.5};
    vector3f v = shader->model->vertices[shader->model->triangles[face+vert]];
    //vector3f t = shader->model->textures[shader->model->triangles[face+vert]-1];
    vector3f n = shader->model->normals[shader->model->triangles[face+vert]];
    vector4f position = multiply_mat4f_vec4f(shader->ModelView, (vector4f){v.x, v.y, v.z, 1.}); // in object coordinates
    shader->normal =  multiply_mat4f_vec4f(shader->ModelView, (vector4f){n.x, n.y, n.z, 0.}); 
    //shader->texture =  multiply_mat4f_vec4f(shader->ModelView, (vector4f){t_norm.x, t_norm.y, t_norm.z, 0.}); 
    
    shader->eye = (vector4f){position.x, position.y, position.z, position.w}; //in eye coordinates
    shader->clip = multiply_mat4f_vec4f(shader->Perspective, position); // in clip coordinates



    shader->normal = scale(shader->normal, scaling);
    shader->eye = scale(shader->eye, scaling);
    shader->clip = scale(shader->clip,scaling);
    shader->normal = rotateY(shader->normal, angle);
    shader->eye = rotateY(shader->eye, angle);
    shader->clip = rotateY(shader->clip, angle);


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

vector4f rotateY(vector4f v, double a) {
    
    matrix4f mat =  {cos(a), 0, sin(a), 0,
                     0,  1, 0, 0,
                    -sin(a), 0, cos(a), 0,
                     0, 0, 0, 1};
    return multiply_mat4f_vec4f(mat, v);

}

void rotateX(vector4f *v, double a) {
    
    matrix4f mat =  {1, 0, 0, 0,
                    0, cos(a), -sin(a), 0,
                    0, sin(a), cos(a), 0,
                    0, 0, 0, 1};

    *v  = multiply_mat4f_vec4f(mat, *v);

}


void rotateZ(vector4f *v, double a) {
    
    matrix4f mat = { cos(a), -sin(a), 0, 0,
                    sin(a), cos(a), 0, 0,
                    0, 0, 1, 0,
                    0, 0, 0, 1 };

    *v  = multiply_mat4f_vec4f(mat, *v);

}

vector4f scale(vector4f v, vector3f s) {
    matrix4f mat = {s.x, 0, 0, 0,
                    0, s.y,0, 0,
                    0, 0, s.z, 0,
                    0, 0, 0, 1};
    
    return multiply_mat4f_vec4f(mat, v);

}

vector3f *find_normals(vector3f* v, int vertices_size, int* triangles, int triangles_size) {
    vector3f *normals = malloc(vertices_size * sizeof(vector3f));
    
    for(int i = 0; i < triangles_size; i += 3) {
        printf("%d", i);
        //Triangles stored together
        vector3f a = v[triangles[i]];
        vector3f b = v[triangles[i+1]];
        vector3f c = v[triangles[i+2]];

        vector3f ab = subtract_vec3(a, b);
        vector3f ac = subtract_vec3(a, c);
        vector3f bc = subtract_vec3(b, c);
        
        vector3f aN = cross(ab, ac);
        vector3f bN = cross (ab, bc);
        vector3f cN = cross(ac, bc);

        normals[triangles[i]] = aN;
        normals[triangles[i+1]] = bN;
        normals[triangles[i+2]] = cN;

    }

    return normals;
}