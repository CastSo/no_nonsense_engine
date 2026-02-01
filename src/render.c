#include "render.h"
#include <omp.h>

//Fills entire pixel buffer with a color
void clear(const image_view *color_buffer, const vector4f *color) {
    color4ub* ptr = color_buffer->pixels;
    uint32_t size = color_buffer->width * color_buffer->height;
    color4ub fill = to_color4ub(color);
    fill_n(ptr, &fill, size, sizeof(fill));
}

void render_tga(image_view *color_buffer, image_view *img_buffer) {
    color4ub* ptr = color_buffer->pixels;
    uint32_t size = color_buffer->width * color_buffer->height;
    for(int i = 0; i < img_buffer->height; i++) {
        for (int j = 0; j < img_buffer->width; j++) {
            *color_buffer->at(color_buffer, j, i) = img_buffer->pixels[j + i * img_buffer->width];
        }
    }
}


vector3f convert_to_ndc(vector3f vec, int width, int height) {
    return (vector3f) { (1.0f + vec.x) * width/ 2,
                        (1.0f - vec.y) * height/ 2,
                        vec.z};
}


void render_faces(Shader *shader, Model *model, double *zbuffer, image_view* color_buffer, bool is_bf_cull, float move) {

    //Reference vertices by induces
    for (int v = 0; v < (model->triangles_size); v += 9) {
      
        vector4f clip[3];
        vector3f varying_uv[3];
        vector4f norm[3];

        for (int f = 0; f < 3; f++) {
            vector3f vec = model->vertices[model->triangles[v+(f*3)]];
            vector4f position = multiply_mat4f_vec4f(shader->ModelView, (vector4f){vec.x, vec.y, vec.z, 1.});
            clip[f] = multiply_mat4f_vec4f(shader->Perspective, position); // in clip coordinates
            
            //Uses vt from model
            varying_uv[f] = model->textures[model->triangles[v+(f*3+1)]];
            vector3f n = model->textures[model->triangles[v+(f*3+2)]];
            norm[f] = multiply_mat4f_vec4f(inverse_mat4f(shader->ModelView), (vector4f){n.x, n.y, n.z, 0});
            
        }

        

        triangle3D(shader, model, zbuffer, clip, varying_uv, norm, color_buffer, is_bf_cull);
    }
    
}

void render_wireframe(Model* model, image_view* color_buffer) {
    int width_scale = 800;
    int height_scale = 800;
    int yoffset = 200;


    for (int i = 0; i < (model->triangles_size); i += 3) {
        
        int ax = model->vertices[model->triangles[i]-1].x;
        int ay = model->vertices[model->triangles[i]-1].y;
        int az = model->vertices[model->triangles[i]-1].z;
        int bx = model->vertices[model->triangles[i+1]-1].x;
        int by = model->vertices[model->triangles[i+1]-1].y;
        int bz = model->vertices[model->triangles[i+1]-1].z;
        int cx = model->vertices[model->triangles[i+2]-1].x;
        int cy = model->vertices[model->triangles[i+2]-1].y;
        int cz = model->vertices[model->triangles[i+2]-1].z;

        vector4f green = {0.0f, 1.0f, 0.0f, 1.0f};
        
        line(ax, ay, bx, by, color_buffer, &green);
        line(bx, by, cx, cy, color_buffer, &green);
        line(cx, cy, ax, ay, color_buffer, &green);
    }
}

void sort_y_coordinates(vector3f* vectors, int n) {
    int i, j;
    bool is_swapped;
    for (int i = 0; i < n-1; i++){
        is_swapped = false;
        for (int j = 0; j < n - i - 1; j++)
        {
            
            if (vectors[j].y > vectors[j+1].y) {
                vector3f tmp = vectors[j];
                vectors[j] = vectors[j+1];
                vectors[j+1] = tmp;
                is_swapped = true;
            } 
        }

        if (is_swapped == false)
            break;
        
    }
}

void render_gui(image_view* color_buffer, mu_Rect dst, mu_Rect src, mu_Color color) {
    float x = src.x / (float) ATLAS_WIDTH;
    float y = src.y / (float) ATLAS_HEIGHT;
    float w = src.w / (float) ATLAS_WIDTH;
    float h = src.h / (float) ATLAS_HEIGHT;
    vector3f vertices[4] = {
        (vector3f){dst.x,        dst.y,       1.0f},
        (vector3f){dst.x+dst.w,  dst.y,       1.0f},
        (vector3f){dst.x,        dst.y+dst.h,       1.0f},
        (vector3f){dst.x+dst.w,        dst.y+dst.h,       1.0f}
    };

    vector3f textures[4] = {
        (vector3f){x,    y,   1.0f},
        (vector3f){x+w,  y,   1.0f},
        (vector3f){x,    y+h,       1.0f},
        (vector3f){x+w,    y+h,       1.0f}
    }; 

    int indices[6] = {0, 1, 2,
                     2, 3, 1,}; 
    color4ub tri_color = {color.r, color.g, color.b, color.a};

    vector3f vec[3];
    vector3f tex[3];
    //Reference vertices by induces
    for (int v = 0; v < 6; v += 3) {
        for (int f = 0; f < 3; f++) {
            vec[f] = vertices[indices[v+f]];
        }

        triangle2D(color_buffer, vec, tri_color, false);
    }

}

void line(int ax, int ay, int bx, int by, image_view *color_buffer, vector4f *color) {
    bool steep = fabsf(ax-bx) < fabsf(ay-by);
    //If steep (more vertical than horizontal) transpose the image to make line more horizontal
    if (steep) {
        swap_int(&ax, &ay);
        swap_int(&bx, &by);
    }

    //Make it left-to-right
    if (ax > bx) {
        swap_int(&ax, &bx);
        swap_int(&ay, &by);
    }

    int y = ay;
    int ierror = 0;

    for (int x=ax; x <= bx; x++) {
        //printf("%d, %d", x, y);

        if(steep) //de-transpose if steep
        {    

            *color_buffer->at(color_buffer, y, x) = to_color4ub(color);
        } else { 

            *color_buffer->at(color_buffer, x, y) = to_color4ub(color);
        }
        ierror += 2 * fabsf(by-ay); //measures error commited when y is more horizontal than vertical
        if (ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx-ax);
        }

    }
    
}



void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, image_view *color_buffer, vector4f *color) {
    vector3f* vectors = (vector3f *)malloc(3* sizeof(vector3f));
    vectors[0].x = ax;
    vectors[0].y = ay;
    vectors[1].x = bx;
    vectors[1].y = by;
    vectors[2].x = cx;
    vectors[2].y = cy;

    sort_y_coordinates(vectors, 3);
    vector3f vector_min = vectors[0];
    vector3f vector_mid = vectors[1];
    vector3f vector_max = vectors[2];
    free(vectors);


    //printf("%f, %f, %f \n", vector_min.y, vector_mid.y, vector_max.y);
    //Skips undefined slope
   if(vector_max.x - vector_min.x == 0 )
    {
        
        //printf("%f, %f, %f \n", vector_min.x, vector_mid.x, vector_max.x);    
        return;
    }

    //y = mx+b
    double m1 = (vector_max.y - vector_min.y)/(vector_max.x - vector_min.x);
    double b1 = vector_min.y - (vector_min.x * m1);

    //Skips undefined slope
    if (vector_max.x - vector_mid.x == 0)
        return;

    double m2 = (vector_max.y - vector_mid.y)/(vector_max.x - vector_mid.x);
    double b2 = vector_mid.y - (vector_mid.x * m2);
    //Loops draw two triangles
    for(int row = vector_mid.y; row < vector_max.y; row++){
        int y = row;
        int Pt1_x = (y-b1) / m1;
        int Pt2_x = (y-b2) / m2;

        line(Pt1_x, y, Pt2_x, y, color_buffer, color);
    
    }
    
    //Skips undefined slope
    if (vector_mid.x - vector_min.x == 0)
        return;

    double m3 = (vector_mid.y - vector_min.y)/(vector_mid.x - vector_min.x);
    double b3 = vector_min.y - (vector_min.x * m3);
    for(int row = vector_min.y; row < vector_mid.y; row++){
        int y = row;
        int Pt1_x = (y-b1) / m1;
        int Pt2_x = (y-b3) / m3;

        line(Pt1_x, y, Pt2_x, y, color_buffer, color);
        
     }

     

}

double signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
}

//Uses bounding box rasterization
void triangle3D(Shader *shader,  Model *model, double *zbuffer, vector4f clip[3], vector3f varying_uv[3], vector4f norm[3], image_view *color_buffer, bool is_backface_cull) {

    vector3f sun_direction = shader->light->direction;
    vector3f cam_pos = shader->camera->position;


    vector4f ndc[3] = {
        { clip[0].x / clip[0].w, clip[0].y / clip[0].w, clip[0].z / clip[0].w, 1.0f },
        { clip[1].x / clip[1].w, clip[1].y / clip[1].w, clip[1].z / clip[1].w, 1.0f },
        { clip[2].x / clip[2].w, clip[2].y / clip[2].w, clip[2].z / clip[2].w, 1.0f }
    };

    vector4f v[3] = {
        multiply_mat4f_vec4f(shader->Viewport, ndc[0]), 
        multiply_mat4f_vec4f(shader->Viewport, ndc[1]), 
        multiply_mat4f_vec4f(shader->Viewport, ndc[2])};
    
    //Triangle ABC in v coordinates   
    matrix3f ABC = {
        v[0].x, v[0].y, 1.,
        v[1].x, v[1].y, 1., 
        v[2].x, v[2].y, 1.      
    };


    //backface culling
    if(determinant(ABC) < 1 && is_backface_cull) return;

    int bbminx = fmin(fmin(v[0].x, v[1].x), v[2].x);
    int bbminy = fmin(fmin(v[0].y, v[1].y), v[2].y);
    int bbmaxx = fmax(fmax(v[0].x, v[1].x), v[2].x);
    int bbmaxy = fmax(fmax(v[0].y, v[1].y), v[2].y);


   #pragma omp parallel for
    for (int x = fmax(bbminx, 0); x <= fmin(bbmaxx, color_buffer->width-1); x++) {
        for (int y = fmax(bbminy,0); y <= fmin(bbmaxy, color_buffer->height-1); y++) {

            int normal_y = color_buffer->height-y-1;
            //Barycentric coordinates
            vector3f bc = multiply_mat3f_vec3f((inverse_mat3f(ABC)), (vector3f){(double)x, (double) y, 1.});
            //Checks if pixel outside triangle
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) 
                continue;
            

            double z = dot_vec3f(bc, (vector3f){ndc[0].z, ndc[1].z, ndc[2].z});
            //Discard pixel p because inferior to z;
            if (z <= zbuffer[x+normal_y*color_buffer->width])
                continue;

            zbuffer[x+normal_y*color_buffer->width] = z;

            //Setup normals in tangent space
            vector4f e1 = subtract_vec4f(clip[1], clip[0]);
            vector4f e0 = subtract_vec4f(clip[2], clip[0]);
            matrix2x4f E = {e0.x, e0.y, e0.z, 0.f, 
                            e1.x, e1.y, e1.z, 0.f};
            vector3f u0 = subtract_vec3f(varying_uv[1], varying_uv[0]);
            vector3f u1 = subtract_vec3f(varying_uv[2], varying_uv[0]);
            matrix2f U = {u0.x, u0.y,
                        u1.x, u1.y};
            matrix2f invert_U = inverse_mat2f(U);
            matrix2x4f T = multiply_mat4f_mat2x4f(invert_U, E);
            
            vector4f interpolated_norm =  normalize_vec4f(add_vec4f(add_vec4f(scale_vec4f(norm[0], bc.x), scale_vec4f(norm[1],bc.y)), scale_vec4f(norm[2],bc.z)));
            matrix4f D = {T.n00, T.n01, T.n02, T.n03, // tangent vector
                         T.n10, T.n11, T.n12, T.n13, //bitangent vector
                        interpolated_norm.x, interpolated_norm.y, interpolated_norm.z, interpolated_norm.w,
                        0, 0, 0, 1
                        };
                        
            vector3f uv = add_vec3f(add_vec3f(scale_vec3f(varying_uv[0], bc.x), scale_vec3f(varying_uv[1], bc.y)), scale_vec3f(varying_uv[2], bc.z));


            vector4f nm = normal(model->header_uv, model->uv, (vector2f){uv.x, uv.y});
            
            vector4f vec_n_nm = normalize_vec4f(multiply_mat4f_vec4f(transpose_mat4f(D), nm));

            vector4f vec_l = normalize_vec4f( (vector4f){sun_direction.x, sun_direction.y, sun_direction.z, 0.0f}); // direction toward sun
            
            int e = 35;
            vector4f vec_v = normalize_vec4f((vector4f){cam_pos.x, cam_pos.y, cam_pos.z, 0.0f}); //fragment to sun
            vector4f vec_r = normalize_vec4f(subtract_vec4f(scale_vec4f(scale_vec4f(vec_n_nm, dot_vec4f(vec_n_nm, vec_l)), 2), vec_l)); //reflection of sun
           // double specular = pow(fmax(0, dot_vec4f(vec_r, vec_v)), e); 
            double diffuse = fmax(0, dot_vec4f(vec_n_nm, vec_l));
            double ambient = .3;
            color4ub spec_color = sample2D(model->header_specular, model->specular, (vector2f){uv.x, uv.y});
            color4ub diff_color = sample2D(model->header_diffuse, model->diffuse, (vector2f){uv.x, uv.y});
            double specular = (.5+2.*spec_color.r/255.) * pow(fmax(0, dot_vec4f(vec_r, vec_v)), e);


            //vector3f color = add_vec3f((vector3f){spec.x, spec.y, spec.z}, (vector3f){diff.x, diff.y ,diff.z});
            

            //(vector4f)color =  normal(model->header_diffuse, model->diffuse, (vector2f){uv.x, uv.y});
            
            double phong = ambient + diffuse + specular;
            // //printf("%f\n", phong);
           
            *color_buffer->at(color_buffer, x, normal_y) = (color4ub) {phong * diff_color.r, phong * diff_color.g, phong * diff_color.b,  model->color.w};
            //*color_buffer->at(color_buffer, x, normal_y) = (color4ub) {color.x, color.y, color.z, 255.0f};
        }
    }

}

void triangle2D(image_view* color_buffer, vector3f v[3], color4ub color, bool is_backface_cull) {
    matrix3f ABC = {
        v[0].x, v[0].y, 1.,
        v[1].x, v[1].y, 1., 
        v[2].x, v[2].y, 1.      
    };


    //backface culling
    if(determinant(ABC) < 1 && is_backface_cull) return;

    int bbminx = fmin(fmin(v[0].x, v[1].x), v[2].x);
    int bbminy = fmin(fmin(v[0].y, v[1].y), v[2].y);
    int bbmaxx = fmax(fmax(v[0].x, v[1].x), v[2].x);
    int bbmaxy = fmax(fmax(v[0].y, v[1].y), v[2].y);


   #pragma omp parallel for
    for (int x = fmax(bbminx, 0); x <= fmin(bbmaxx, color_buffer->width-1); x++) {
        for (int y = fmax(bbminy,0); y <= fmin(bbmaxy, color_buffer->height-1); y++) {
             //Barycentric coordinates
            vector3f bc = multiply_mat3f_vec3f((inverse_mat3f(ABC)), (vector3f){(double)x, (double) y, 1.});
            //Checks if pixel outside triangle
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) 
                continue;

            int normal_y = color_buffer->height-y-1;

            *color_buffer->at(color_buffer, x, y) = color;
            //printf("%d, %d, %d\n", color.r, color.g, color.b);
        }
    }

}

void triangle2D_texture(image_view* color_buffer, vector3f v[3], vector3f tex[3], color4ub color, bool is_backface_cull) {
    matrix3f ABC = {
        v[0].x, v[0].y, 1.,
        v[1].x, v[1].y, 1., 
        v[2].x, v[2].y, 1.      
    };


    //backface culling
    if(determinant(ABC) < 1 && is_backface_cull) return;

    int bbminx = fmin(fmin(v[0].x, v[1].x), v[2].x);
    int bbminy = fmin(fmin(v[0].y, v[1].y), v[2].y);
    int bbmaxx = fmax(fmax(v[0].x, v[1].x), v[2].x);
    int bbmaxy = fmax(fmax(v[0].y, v[1].y), v[2].y);


   #pragma omp parallel for
    for (int x = fmax(bbminx, 0); x <= fmin(bbmaxx, color_buffer->width-1); x++) {
        for (int y = fmax(bbminy,0); y <= fmin(bbmaxy, color_buffer->height-1); y++) {
             //Barycentric coordinates
            vector3f bc = multiply_mat3f_vec3f((inverse_mat3f(ABC)), (vector3f){(double)x, (double) y, 1.});
            //Checks if pixel outside triangle
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) 
                continue;

            vector3f m = add_vec3f(add_vec3f(scale_vec3f(tex[0], bc.x), scale_vec3f(tex[1], bc.y)), scale_vec3f(tex[2], bc.z));

            int normal_y = color_buffer->height-y-1;

            *color_buffer->at(color_buffer, x, y) = (color4ub){};
            //printf("%d, %d, %d\n", color.r, color.g, color.b);
        }
    }

}

