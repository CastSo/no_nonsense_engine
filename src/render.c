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

        if(steep) //de-transpose if steep
            *color_buffer->at(color_buffer, y, x) = to_color4ub(color);
        else 
            *color_buffer->at(color_buffer, x, y) = to_color4ub(color);
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
    printf("%d /n", model->triangles_size);
    printf("%d", face_i);
    return model;
}

void render_wireframe(Model* model, image_view* color_buffer) {
    for (int i = 0; i < (model->triangles_size); i += 3) {
        //printf("%d \n", i);
        int width_scale = 800;
        int height_scale = 800;
        int ax = (1.0f + model->vertices[model->triangles[i]-1].x) * width_scale/ 2;
        int ay = (1.0f - model->vertices[model->triangles[i]-1].y)  * height_scale / 2;
        int bx = (1.0f + model->vertices[model->triangles[i+1]-1].x) * width_scale/ 2;
        int by = (1.0f - model->vertices[model->triangles[i+1]-1].y ) * height_scale / 2;
        int cx = (1.0f + model->vertices[model->triangles[i+2]-1].x ) * width_scale/ 2;
        int cy = (1.0f - model->vertices[model->triangles[i+2]-1].y)  * height_scale/ 2;



        vector4f red = {1.0f, 0.0f, 0.0f, 1.0f};

        line(ax, ay, bx, by, color_buffer, &red);
        line(bx, by, cx, cy, color_buffer, &red);
        line(cx, cy, ax, ay, color_buffer, &red);
    }
}

