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

    for (int v = 0; v < (model->triangles_size); v += 3) {
      
        vector4f clip[3];
        vector3f varying_uv[3];

        for (int f = 0; f < 3; f++) {
            vector3f vec = model->vertices[model->triangles[f+v]];
            vector4f position = multiply_mat4f_vec4f(shader->ModelView, (vector4f){vec.x, vec.y, vec.z, 1.});
            clip[f] = multiply_mat4f_vec4f(shader->Perspective, position); // in clip coordinates
            
            //Uses vt from model
            varying_uv[f] = model->textures[model->triangles[f+v]];

            //printf("%f, %f, %f\n", varying_uv[f].x, varying_uv[f].y, varying_uv[f].z);
        }

        
      triangle(shader, model, zbuffer, clip, varying_uv, color_buffer, is_bf_cull);
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
void triangle(Shader *shader,  Model *model, double *zbuffer, vector4f clip[3], vector3f varying_uv[3],  image_view *color_buffer, bool is_backface_cull) {
    
    vector3f sun_direction = shader->light->direction;
    vector3f cam_pos = shader->camera->position;


    vector4f ndc[3] = {
        { clip[0].x / clip[0].w, clip[0].y / clip[0].w, clip[0].z / clip[0].w, 1.0f },
        { clip[1].x / clip[1].w, clip[1].y / clip[1].w, clip[1].z / clip[1].w, 1.0f },
        { clip[2].x / clip[2].w, clip[2].y / clip[2].w, clip[2].z / clip[2].w, 1.0f }
    };

    vector4f screen[3] = {
        multiply_mat4f_vec4f(shader->Viewport, ndc[0]), 
        multiply_mat4f_vec4f(shader->Viewport, ndc[1]), 
        multiply_mat4f_vec4f(shader->Viewport, ndc[2])};
    
    //Triangle ABC in screen coordinates   
    matrix3f ABC = {
        screen[0].x, screen[0].y, 1.,
        screen[1].x, screen[1].y, 1., 
        screen[2].x, screen[2].y, 1.      
    };


    //backface culling
    if(determinant(ABC) < 1 && is_backface_cull) return;

    int bbminx = fmin(fmin(screen[0].x, screen[1].x), screen[2].x);
    int bbminy = fmin(fmin(screen[0].y, screen[1].y), screen[2].y);
    int bbmaxx = fmax(fmax(screen[0].x, screen[1].x), screen[2].x);
    int bbmaxy = fmax(fmax(screen[0].y, screen[1].y), screen[2].y);

   #pragma omp parallel for
    for (int x = fmax(bbminx, 0); x <= fmin(bbmaxx, color_buffer->width-1); x++) {
        for (int y = fmax(bbminy,0); y <= fmin(bbmaxy, color_buffer->height-1); y++) {
            int normal_y = color_buffer->height-y-1;
            //Barycentric coordinates
            vector3f bc = multiply_mat3f_vec3f((inverse(ABC)), (vector3f){(double)x, (double) y, 1.});
            //Checks if pixel outside triangle
            if (bc.x < 0 || bc.y < 0 || bc.z < 0) 
                continue;

            vector3f uv = add_vec3f(add_vec3f(scale_vec3f(varying_uv[0], bc.x), scale_vec3f(varying_uv[1], bc.y)), scale_vec3f(varying_uv[2], bc.z));

            vector4f n = multiply_mat4f_vec4f(shader->ModelView, normal(*model, (vector2f){uv.x, uv.y}));
            //vector4f n = (vector4f){uv.x, uv.y, 0.0f, 0.0f};
            vector4f vec_n = normalize_vec4f(n);
            vector4f vec_l = normalize_vec4f( (vector4f){sun_direction.x, sun_direction.y, sun_direction.z, 0.0f}); // direction toward sun
            
            
            int e = 35;
            vector4f vec_v = normalize_vec4f((vector4f){cam_pos.x, cam_pos.y, cam_pos.z, 0.0f}); //fragment to sun
            //vector4f vec_r = (subtract_vec4f(scale_vec4f(scale_vec4f(vec_n,2), dot_vec4f(vec_n, vec_l)), vec_l)); //reflection of sun
            vector4f vec_r = normalize_vec4f(subtract_vec4f(scale_vec4f(scale_vec4f(vec_n, dot_vec4f(vec_n, vec_l)), 2), vec_l)); //reflection of sun
            double specular = pow(fmax(0, dot_vec4f(vec_r, vec_v)), e); 
            double diff = fmax(0, dot_vec4f(vec_n, vec_l));
            double ambient = .3;

            double z = dot_vec3f(bc, (vector3f){ndc[0].z, ndc[1].z, ndc[2].z});
             //y growing downward
            

            //Discard pixel p because inferior to z;
            if (z <= zbuffer[x+normal_y*color_buffer->width])
                continue;

            zbuffer[x+normal_y*color_buffer->width] = z;


            
            double phong = ambient + .4*diff + .9*specular;
            //printf("%f\n", phong);
           
            *color_buffer->at(color_buffer, x, normal_y) = (color4ub) {phong * model->color.x , phong * model->color.y, phong * model->color.z,  model->color.w};
            
        }
    }


}

