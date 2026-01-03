#include "render.h"

//Fills entire pixel buffer with a color
void clear(const image_view *color_buffer, const vector4f *color) {
    color4ub* ptr = color_buffer->pixels;
    uint32_t size = color_buffer->width * color_buffer->height;
    color4ub fill = to_color4ub(color);
    fill_n(ptr, &fill, size, sizeof(fill));
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

struct Model* read_model_lines(char *file_name) {
    FILE *fptr = fopen(file_name, "r");
    if (fptr == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    Model *model = (Model *)malloc(sizeof(Model));

    int vertices_size;
    model->vertices = (vector3f *)malloc(1000000 * sizeof(vector3f));
    int triangles_size;
    model->triangles = (int*)malloc(1000000 * sizeof(int));

    char *buffer;
    size_t buffer_size = 256;
    size_t characters;

    char *line = NULL;
    char *prev_line;
    const char *delim = " ";
    int line_count = 0;
    bool vertices_end = false;

    buffer = (char *)malloc(buffer_size * sizeof(char));

    int vert_i = 0;
    int face_i = 0;

    char* saveptr1;
    
    //Checks only vertices
    while (getline(&buffer, &buffer_size, fptr) != -1) 
    {
        
        line = strtok_r(buffer, delim, &saveptr1);
       
        if (strcmp(line, "v") == 0)
        {    
            //Skips char v
            line = strtok_r(NULL, delim, &saveptr1);

            char *endptr;
            model->vertices[vert_i].x = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);
            
            endptr = NULL;
            model->vertices[vert_i].y = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);

            endptr = NULL;
            model->vertices[vert_i].z = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);

            
            //printf("%f, %f, %f\n", model->vertices[vert_i].x, model->vertices[vert_i].y, model->vertices[vert_i].z);
            vert_i++;
            
            
        } else if (strcmp(line, "f") == 0) {
            char *saveptr2;
            
            //Only checks the first index of each word
            while (prev_line != NULL) {      
                //Skips char f
                char *current_vert = strtok_r(prev_line, "/", &saveptr2);

                if(atoi(current_vert) != 0) {
                    model->triangles[face_i] = atoi(current_vert);
                    //printf("%d\n",  model->triangles[face_i]);
                    face_i++;
                }
                

                prev_line = strtok_r(NULL, " ", &saveptr1);
            }

        } 
        
        prev_line = strtok_r(buffer, delim, &saveptr1);


    }

    fclose(fptr);
    free(buffer);
    model->vertices_size = vert_i;
    model->triangles_size = face_i;

    return model;
}

void render_faces(Model* model, image_view* color_buffer, vector4f* rand_colors) {
    int width_scale = 800;
    int height_scale = 800;
    int yoffset = 200;


    for (int i = 0; i < (model->triangles_size); i += 3) {
        
        int ax = (1.0f + model->vertices[model->triangles[i]-1].x) * width_scale/ 2;
        int ay = yoffset + (1.0f - model->vertices[model->triangles[i]-1].y)  * height_scale / 2;
        int bx = (1.0f + model->vertices[model->triangles[i+1]-1].x) * width_scale/ 2;
        int by = yoffset + (1.0f - model->vertices[model->triangles[i+1]-1].y ) * height_scale / 2;
        int cx = (1.0f + model->vertices[model->triangles[i+2]-1].x ) * width_scale/ 2;
        int cy = yoffset + (1.0f - model->vertices[model->triangles[i+2]-1].y)  * height_scale/ 2;



        vector4f color = rand_colors[i/3];
        //triangle(ax, ay, bx, by, cx, cy, color_buffer, &color);
    }
    //free(color_buffer);
}

void render_wireframe(Model* model, image_view* color_buffer) {
    int width_scale = 800;
    int height_scale = 800;
    int yoffset = 200;


    for (int i = 0; i < (model->triangles_size); i += 3) {
        
        int ax = (1.0f + model->vertices[model->triangles[i]-1].x) * width_scale/ 2;
        int ay = yoffset + (1.0f - model->vertices[model->triangles[i]-1].y)  * height_scale / 2;
        int bx = (1.0f + model->vertices[model->triangles[i+1]-1].x) * width_scale/ 2;
        int by = yoffset + (1.0f - model->vertices[model->triangles[i+1]-1].y ) * height_scale / 2;
        int cx = (1.0f + model->vertices[model->triangles[i+2]-1].x ) * width_scale/ 2;
        int cy = yoffset + (1.0f - model->vertices[model->triangles[i+2]-1].y)  * height_scale/ 2;

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
void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, image_view *color_buffer) {
    int bbminx = fmin(fmin(ax, bx), cx);
    int bbminy = fmin(fmin(ay, by), cy);
    int bbmaxx = fmax(fmax(ax, bx), cx);
    int bbmaxy = fmax(fmax(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area<1) return;
    
    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta  = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha<0.0f || beta<0.0f || gamma<0.0f) 
                continue; 

            if (alpha<0.1f || beta<0.1f || gamma<0.1f) 
            {    
                color4ub z = (color4ub) {(alpha * az), (beta * bz), (gamma * cz), 0.0f};
                *color_buffer->at(color_buffer, x, y) = z;
            }
        }
    }


}