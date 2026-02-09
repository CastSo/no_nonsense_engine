#include "render.h"
#include <omp.h>


vector4f edge_init(Edge* self, vector2f v0, vector2f v1, vector2f origin) {
    //Edge
    float A = v0.y - v1.y, B = v1.x - v0.x;
    float C = v0.x * v1.y - v0.y * v1.x;

    //Step deltas
    self->one_step_x = scale_vec4f((vector4f){A, A, A, A}, self->step_x_size);
    self->one_step_y = scale_vec4f((vector4f){B, B, B, B}, self->step_y_size);

    /*
    (0,0) | (1,0)
    ____________
    (0,1) | (1,1)
    */
    //x/y values for initial pixel block (2x2)
    vector4f x = add_vec4f((vector4f){origin.x, origin.x, origin.x, origin.x}, (vector4f){0, 1, 1, 0});
    vector4f y = add_vec4f((vector4f){origin.y, origin.y, origin.y, origin.y}, (vector4f){0, 1, 0, 1});

    //Edge function values at origin
    return add_vec4f(add_vec4f(multiply_vec4f((vector4f){A, A, A, A}, x), multiply_vec4f((vector4f){B, B, B, B}, y)), (vector4f){C, C, C, C});
}

//Fills entire pixel buffer with a color
void clear(const image_view *color_buffer, const vector4f *color) {
    color4ub* ptr = color_buffer->pixels;
    uint32_t size = color_buffer->width * color_buffer->height;
    color4ub fill = to_color4ub(color);
    fill_n(ptr, &fill, size, sizeof(fill));
}

//Renders entire tga image
void render_tga(image_view *color_buffer, image_view *img_buffer) {
    color4ub* ptr = color_buffer->pixels;
    uint32_t size = color_buffer->width * color_buffer->height;
    for(int i = 0; i < img_buffer->height; i++) {
        for (int j = 0; j < img_buffer->width; j++) {
            *color_buffer->at(color_buffer, j, i) = img_buffer->pixels[j + i * img_buffer->width];
        }
    }
}



void render_faces(Shader *shader, Model *model, float *zbuffer, image_view* color_buffer, bool is_bf_cull, float move) {

    //Reference vertices by induces
    for (int v = 0; v < (model->triangles_size); v += 9) {


        for (int f = 0; f < 3; f++) {
            vector3f vec = model->vertices[model->triangles[v+(f*3)]];
            vector4f local = (vector4f){vec.x, vec.y, vec.z, 1.};
            
            //Transformations in local space
            //local = rotateY(local, model->angle);

            vector4f position = multiply_mat4f_vec4f(shader->ModelView, local);
            shader->clip[f] = multiply_mat4f_vec4f(shader->Perspective, position); // in clip coordinates
            
            
            //Uses vt from model
            shader->varying_uv[f] = model->textures[model->triangles[v+(f*3+1)]];
            vector3f n = model->textures[model->triangles[v+(f*3+2)]];
            shader->norm[f] = multiply_mat4f_vec4f(inverse_mat4f(shader->ModelView), (vector4f){n.x, n.y, n.z, 0});

        }

        

        triangle3D(shader, model, zbuffer, color_buffer, is_bf_cull);
    }
    
}

void render_wireframe(Model* model, image_view* color_buffer) {
    int width_scale = 800;
    int height_scale = 800;
    int yoffset = 200;


    for (int i = 0; i < (model->triangles_size); i += 9) {
        vector3f a = 
        {1.0f + model->vertices[model->triangles[i]].x * 600/ 2,
         1.0f - model->vertices[model->triangles[i]].y * 600 / 2,
         model->vertices[model->triangles[i]].z};
        vector3f b =
        {1.0f + model->vertices[model->triangles[i+3]].x * 600/ 2,
         1.0f - model->vertices[model->triangles[i+3]].y * 600 / 2,
         model->vertices[model->triangles[i+3]].z};
        vector3f c =
        {1.0f + model->vertices[model->triangles[i+6]].x * 600/ 2,
         1.0f - model->vertices[model->triangles[i+6]].y * 600 / 2,
         model->vertices[model->triangles[i+6]].z};

        color4ub green = {0.0f, 255.0f, 0.0f};
        
        line(a.x, a.y, b.x, b.y, color_buffer, green);
        line(b.x, b.y, c.x, c.y, color_buffer, green);
        line(c.x, c.y, a.x, a.y, color_buffer, green);
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

void render_gui_texture(image_view* color_buffer, mu_Rect dst, mu_Rect src, mu_Color color) {
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
            tex[f] = textures[indices[v+f]];
            //printf("%f, %f\n", tex[f].x, tex[f].y);    
        }
        

        triangle2D_texture(color_buffer, vec, tex, tri_color, false);
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


    int indices[6] = {0, 1, 2,
                     2, 3, 1,}; 
    color4ub tri_color = {color.r, color.g, color.b, color.a};

    vector3f vec[3];
    //Reference vertices by induces
    for (int v = 0; v < 6; v += 3) {
        for (int f = 0; f < 3; f++) {
            vec[f] = vertices[indices[v+f]];
            //printf("%f, %f\n", tex[f].x, tex[f].y);    
        }
        

        triangle2D(color_buffer, vec, tri_color, false);
    }

}

void line(int ax, int ay, int bx, int by, image_view *color_buffer, color4ub color) {
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

            *color_buffer->at(color_buffer, y, x) = color;
        } else { 

            *color_buffer->at(color_buffer, x, y) = color;
        }
        ierror += 2 * fabsf(by-ay); //measures error commited when y is more horizontal than vertical
        if (ierror > bx - ax) {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx-ax);
        }

    }
    
}



//Uses bounding box rasterization
void triangle3D(Shader *shader,  Model *model, float *zbuffer,  image_view *color_buffer, bool is_backface_cull) {

    vector3f sun_direction = shader->light.direction;
    vector3f cam_pos = shader->camera.position;


    vector4f ndc[3] = {
        { shader->clip[0].x / shader->clip[0].w, shader->clip[0].y / shader->clip[0].w, shader->clip[0].z / shader->clip[0].w, 1.0f },
        { shader->clip[1].x / shader->clip[1].w, shader->clip[1].y / shader->clip[1].w, shader->clip[1].z / shader->clip[1].w, 1.0f },
        { shader->clip[2].x / shader->clip[2].w, shader->clip[2].y / shader->clip[2].w, shader->clip[2].z / shader->clip[2].w, 1.0f }
    };

    //Screen space
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

    //Barycentric coordinates at min x and min y corner
    vector2f p = {bbminx, bbminy};
    Edge e01, e12, e20;
    int step_x_size = 2;
    int step_y_size = 2;
    e01.step_x_size = step_x_size;
    e01.step_y_size = step_y_size;
    e12.step_x_size = step_x_size;
    e12.step_y_size = step_y_size;
    e20.step_x_size = step_x_size;
    e20.step_y_size = step_y_size;

    //Sub triangles
    vector4f w0_row = edge_init(&e12, (vector2f){v[1].x, v[1].y}, (vector2f){v[2].x, v[2].y}, p);
    vector4f w1_row = edge_init(&e20, (vector2f){v[2].x, v[2].y}, (vector2f){v[0].x, v[0].y}, p);
    vector4f w2_row = edge_init(&e01, (vector2f){v[0].x, v[0].y}, (vector2f){v[1].x, v[1].y}, p);
    double twice_total_area = twice_triangle_area((vector2f){v[0].x, v[0].y},(vector2f){v[1].x, v[1].y}, (vector2f){v[2].x, v[2].y});
    

    //#pragma omp parallel for
    for (p.y = fmax(bbminy, 0); p.y <=  fmin(bbmaxy, color_buffer->height-1); p.y += step_y_size){
        vector4f w0 = w0_row;
        vector4f w1 = w1_row;
        vector4f w2 = w2_row;

        for (p.x = fmax(bbminx, 0); p.x <=  fmin(bbmaxx, color_buffer->width-1); p.x += step_x_size){
            //Groups by 4 pixels wide and 1 pixel high
            bool mask[4] = {(w0.x >= 0 && w1.x >= 0 && w2.x >= 0), 
                            (w0.y >= 0 && w1.y >= 0 && w2.y >= 0), 
                            (w0.z >= 0 && w1.z >= 0 && w2.z >= 0), 
                            (w0.w >= 0 && w1.w >= 0 && w2.w >= 0)}; 
            bool any_mask = mask[0] || mask[1] || mask[2]  || mask[3];
            vector3f all_bc[4] = {(vector3f){w0.x, w1.x, w2.x}, 
                                  (vector3f){w0.y, w1.y, w2.y},  
                                  (vector3f){w0.z, w1.z, w2.z}, 
                                  (vector3f){w0.w, w1.w, w2.w}}; 

            if(any_mask){
                //#pragma omp parallel for
                //Cycles through 2x2
                for(int i = 0; i <= 1; i++){
                    int normal_y = color_buffer->height-(p.y+i)-1;  
                    float x = fmin(p.x+i, color_buffer->width-1);
                    float y = fmin(normal_y, color_buffer->height-1);

                    if(!mask[i])
                        continue;
                    vector3f bc = scale_vec3f((vector3f){all_bc[i].x, all_bc[i].y, all_bc[i].z}, 1/twice_total_area);
                    
                    float z = dot_vec3f(bc, (vector3f){shader->clip[0].z, shader->clip[1].z, shader->clip[2].z});
                    //Discard pixel p because inferior to z;
                    if (z <= zbuffer[(int)(x+y*color_buffer->width)])
                    {    
                        continue;
                    }

                    zbuffer[(int)(x+y*color_buffer->width)] = z;  
                    render_pixel(shader, model, zbuffer, color_buffer, x, y, bc);

                }
                for(int i = 0; i <= 1; i++){
                    int normal_y = color_buffer->height-(p.y+i)-1;  
                    float x = fmin(p.x+1-i, color_buffer->width-1);
                    float y = fmin(normal_y, color_buffer->height-1);

                    if(!mask[i+2])
                        continue;
                    vector3f bc = scale_vec3f((vector3f){all_bc[i+2].x, all_bc[i+2].y, all_bc[i+2].z}, 1/twice_total_area);
                    
                    float z = dot_vec3f(bc, (vector3f){shader->clip[0].z, shader->clip[1].z, shader->clip[2].z});
                    //Discard pixel p because inferior to z;
                    if (z <= zbuffer[(int)(x+y*color_buffer->width)])
                    {    
                        continue;
                    }

                    zbuffer[(int)(x+y*color_buffer->width)] = z;  
                    render_pixel(shader, model, zbuffer, color_buffer, x, y, bc);

                }
                
            }
            //step to right
            w0 = add_vec4f(w0, e12.one_step_x);
            w1 = add_vec4f(w1, e20.one_step_x);
            w2 = add_vec4f(w2, e01.one_step_x); 
    }
    //row step
    w0_row = add_vec4f(w0_row, e12.one_step_y);
    w1_row = add_vec4f(w1_row, e20.one_step_y);
    w2_row = add_vec4f(w2_row, e01.one_step_y); 
    }

}

void triangle2D(image_view* color_buffer, vector3f v[3], color4ub color, bool is_backface_cull) {
    
    
    matrix3f ABC = {
        v[0].x, v[0].y, 1.,
        v[1].x, v[1].y, 1., 
        v[2].x, v[2].y, 1.      
    };
    vector2i AB = subtract_vec2i((vector2i){v[1].x, v[0].y}, (vector2i){v[0].x, v[1].y});
    vector2i BC = subtract_vec2i((vector2i){v[2].x, v[1].y}, (vector2i){v[1].x, v[2].y});
    vector2i AC = subtract_vec2i((vector2i){v[0].x, v[2].y}, (vector2i){v[2].x, v[0].y});

    //backface culling
    if(determinant(ABC) < 1 && is_backface_cull) return;

    int bbminx = fmin(fmin(v[0].x, v[1].x), v[2].x);
    int bbminy = fmin(fmin(v[0].y, v[1].y), v[2].y);
    int bbmaxx = fmax(fmax(v[0].x, v[1].x), v[2].x);
    int bbmaxy = fmax(fmax(v[0].y, v[1].y), v[2].y);

    //Barycentric coordinates at min x and min y corner
    vector2f p = {bbminx, bbmaxy};
    int w0_row = twice_triangle_area((vector2f){v[1].x, v[1].y}, (vector2f){v[2].x, v[2].y}, p);
    int w1_row = twice_triangle_area((vector2f){v[2].x, v[2].y}, (vector2f){v[0].x, v[0].y}, p);
    int w2_row = twice_triangle_area((vector2f){v[0].x, v[0].y}, (vector2f){v[1].x, v[1].y}, p);

   #pragma omp parallel for
    for (int x = fmax(bbminx, 0); x <= fmin(bbmaxx, color_buffer->width-1); x++) {
        int w0 = w0_row;
        int w1 = w1_row;
        int w2 = w2_row;
        for (int y = fmax(bbminy,0); y <= fmin(bbmaxy, color_buffer->height-1); y++) {
             //Barycentric coordinates
            vector3f bc = multiply_mat3f_vec3f((inverse_mat3f(ABC)), (vector3f){(float)x, (float) y, 1.});
            // //Checks if pixel outside triangle
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) 
                continue;



            int normal_y = color_buffer->height-y-1;

            *color_buffer->at(color_buffer, x, y) = color;

        }

    }

}

float signed_triangle_area(int ax, int ay, int bx, int by, int cx, int cy) {
    return .5*((by-ay)*(bx+ax) + (cy-by)*(cx+bx) + (ay-cy)*(ax+cx));
}

//twice the signed area of a triangle
float twice_triangle_area(const vector2f a, const vector2f b, const vector2f c)
{
    return (b.x-a.x)*(c.y-a.y) - (b.y-a.y)*(c.x-a.x);
}

void triangle2D_texture(image_view* color_buffer, vector3f clip[3], vector3f tex[3], color4ub color, bool is_backface_cull) {

    matrix3f ABC = {
        clip[0].x, clip[0].y, 1.,
        clip[1].x, clip[1].y, 1., 
        clip[2].x, clip[2].y, 1.      
    };
    
    // Triangle setup
    int A01 = clip[0].y - clip[1].y, B01 = clip[1].x - clip[0].x;
    int A12 = clip[1].y - clip[2].y, B12 = clip[2].x - clip[1].x;
    int A20 = clip[2].y - clip[0].y, B20 = clip[0].x - clip[2].x;


    //backface culling
    if(determinant(ABC) < 1 && is_backface_cull) return;
    int bbminx = fmin(fmin(clip[0].x, clip[1].x), clip[2].x);
    int bbminy = fmin(fmin(clip[0].y, clip[1].y), clip[2].y);
    int bbmaxx = fmax(fmax(clip[0].x, clip[1].x), clip[2].x);
    int bbmaxy = fmax(fmax(clip[0].y, clip[1].y), clip[2].y);

    //Barycentric coordinates at min x and min y corner

    #pragma omp parallel for
    
    for (int y = fmax(bbminy,0); y <= fmin(bbmaxy, color_buffer->height-1); y++){


        for (int x = fmax(bbminx, 0); x <= fmin(bbmaxx, color_buffer->width-1); x++)  {

             //Barycentric coordinates
            vector3f bc = multiply_mat3f_vec3f((inverse_mat3f(ABC)), (vector3f){(float)x, (float) y, 1.});
            //Checks if pixel outside triangle
            if(bc.x < 0 || bc.y < 0 || bc.z < 0)
                continue;
             
            vector3f m = add_vec3f(add_vec3f(scale_vec3f(tex[0], bc.x), scale_vec3f(tex[1], bc.y)), scale_vec3f(tex[2], bc.z));
            //Texture coordinates 
            int tx = (int)(m.x * (ATLAS_WIDTH));
            int ty = (int)(m.y * (ATLAS_HEIGHT));

            //CHANGE ATLAS TO PIXEL BUFFER FORMAT LATER
            int id = ty*ATLAS_WIDTH+tx;

            //printf("%d, %d, %d\n", w0, w1, w2);
            *color_buffer->at(color_buffer, x, y) = (color4ub){atlas_texture[id], atlas_texture[id], atlas_texture[id], atlas_texture[id]};
            
        }
        
    }

}



void render_pixel(Shader* shader, Model* model, float* zbuffer, image_view* color_buffer,  float x, float y, vector3f barycoord) {
        //y grows downward
        //int normal_y = color_buffer->height-y-1;  
        // float z = dot_vec3f(barycoord, (vector3f){shader->clip[0].z, shader->clip[1].z, shader->clip[2].z});
        // //Discard pixel p because inferior to z;
        // if (z <= zbuffer[(int)((x)+y*color_buffer->width)])
        // {    
        //     return;
        // }

        // zbuffer[(int)((x)+y*color_buffer->width)] = z;  
        //*******Find fragment colors using normal data and perspective transformation*******
        //Setup normals in tangent space
        vector4f e1 = subtract_vec4f(shader->clip[1], shader->clip[0]);
        vector4f e0 = subtract_vec4f(shader->clip[2], shader->clip[0]);
        matrix2x4f E = {e0.x, e0.y, e0.z, 0.f, 
                        e1.x, e1.y, e1.z, 0.f};
        vector3f u0 = subtract_vec3f(shader->varying_uv[1], shader->varying_uv[0]);
        vector3f u1 = subtract_vec3f(shader->varying_uv[2], shader->varying_uv[0]);
        matrix2f U = {u0.x, u0.y,
                    u1.x, u1.y};
        matrix2f invert_U = inverse_mat2f(U);
        matrix2x4f T = multiply_mat4f_mat2x4f(invert_U, E);
        
        vector4f interpolated_norm =  normalize_vec4f(add_vec4f(add_vec4f(scale_vec4f(shader->norm[0], barycoord.x), scale_vec4f(shader->norm[1],barycoord.y)), scale_vec4f(shader->norm[2],barycoord.z)));
        matrix4f D = {T.n00, T.n01, T.n02, T.n03, // tangent vector
                        T.n10, T.n11, T.n12, T.n13, //bitangent vector
                    interpolated_norm.x, interpolated_norm.y, interpolated_norm.z, interpolated_norm.w,
                    0, 0, 0, 1
                    };
                    
        vector3f uv = add_vec3f(add_vec3f(scale_vec3f(shader->varying_uv[0], barycoord.x), scale_vec3f(shader->varying_uv[1],barycoord.y)), scale_vec3f(shader->varying_uv[2], barycoord.z));


        vector4f nm = normal(model->header_uv, model->uv, (vector2f){uv.x, uv.y});
        
        vector4f vec_n_nm = normalize_vec4f(multiply_mat4f_vec4f(transpose_mat4f(D), nm));

        vector4f vec_l = normalize_vec4f( (vector4f){shader->light.direction.x, shader->light.direction.y, shader->light.direction.z, 0.0f}); // direction toward sun
        
        int e = 35;
        vector4f vec_v = normalize_vec4f((vector4f){shader->camera.position.x, shader->camera.position.y, shader->camera.position.z, 0.0f}); //fragment to sun
        vector4f vec_r = normalize_vec4f(subtract_vec4f(scale_vec4f(scale_vec4f(vec_n_nm, dot_vec4f(vec_n_nm, vec_l)), 2), vec_l)); //reflection of sun

        //Phong colors
        float diffuse = fmax(0, dot_vec4f(vec_n_nm, vec_l));
        float ambient = .3;
        color4ub spec_color = sample2D(model->header_specular, model->specular, (vector2f){uv.x, uv.y});
        color4ub diff_color = sample2D(model->header_diffuse, model->diffuse, (vector2f){uv.x, uv.y});
        float specular = (.5+2.*spec_color.r/255.) * pow(fmax(0, dot_vec4f(vec_r, vec_v)), e);

        
        float phong = ambient + diffuse + specular;
        
        *color_buffer->at(color_buffer, x, y) = (color4ub) {phong * diff_color.r, phong * diff_color.g, phong * diff_color.b,  model->color.w};

}