#include "model.h"


struct Model* read_model_lines(char *file_name) {
    FILE *fptr = fopen(file_name, "r");
    if (fptr == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    Model *model = malloc(sizeof(Model));

    int vertices_size;
    model->vertices = malloc(100000 * sizeof(vector3f));
    int triangles_size;
    model->triangles = malloc(100000 * sizeof(int));
    int normals_size;
    model->normals = malloc(100000 * sizeof(vector3f));
    int texture_size;
    model->textures = malloc(100000 * sizeof(vector3f));


    char *buffer;
    size_t buffer_size = 256;
    size_t characters;

    char *line = NULL;
    char *prev_line = NULL;
    const char *delim = " ";
    int line_count = 0;
    bool vertices_end = false;

    buffer = malloc(buffer_size * sizeof(char));

    int vert_i = 0;
    int face_i = 0;
    int norm_i = 0;
    int texture_i = 0;

    char* saveptr1;

    int width_scale = 800;
    int height_scale = 800;
    int yoffset = 200;
    
    //Checks only vertices
    while (getline(&buffer, &buffer_size, fptr) != -1) 
    {
        
        line = strtok_r(buffer, delim, &saveptr1);
        
        if (strcmp(line, "v") == 0)
        {    
            //Skips char v
            line = strtok_r(NULL, delim, &saveptr1);

            char *endptr;
            double x = strtod(line, &endptr);
            model->vertices[vert_i].x = x;
            
            line = strtok_r(NULL, delim, &saveptr1);
            
            endptr = NULL;
            double y = strtod(line, &endptr);
            model->vertices[vert_i].y = y;
            line = strtok_r(NULL, delim, &saveptr1);

            endptr = NULL;
            model->vertices[vert_i].z = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);

            vert_i++;
            
            
        } else if (strcmp(line, "f") == 0) {
            char *saveptr2;
            int line_vert[64];
            int n = 0;

            //Only checks the first index of each word
            while (prev_line != NULL) {    
                //Skips char f
                char *vert = strtok_r(prev_line, "/", &saveptr2);

                int v = atoi(vert);
                if(v != 0) {
                    //vertices on faces start at 1
                    line_vert[n] = v-1;
                    n += 1;
                }
                //Use later when want to get face data of vn and vt
                // while (vert != NULL){
                //     int v = atoi(vert);
                //     if(v != 0) {
                //         model->triangles[face_i] = v;
                //         printf("%d, ", model->triangles[face_i]);
                //         face_i++;
                //     }
                //     vert = strtok_r(NULL, "/", &saveptr2);
                // }
                
                prev_line = strtok_r(NULL, " ", &saveptr1);

            }

            for (int i = 1; i < n - 1; i++) {
                model->triangles[face_i++] = line_vert[0];
                model->triangles[face_i++] = line_vert[i];
                model->triangles[face_i++] = line_vert[i + 1];
                //printf("%d, %d, %d \n", line_vert[0], line_vert[i], line_vert[i+1]);  
            }



        } else if (strcmp(line, "vn") == 0) {
            //Skips char vn
            line = strtok_r(NULL, delim, &saveptr1);

            char *endptr;
            double x = strtod(line, &endptr);
            model->normals[norm_i].x = x;
            
            line = strtok_r(NULL, delim, &saveptr1);
            
            endptr = NULL;
            double y = strtod(line, &endptr);
            model->normals[norm_i].y = y;
            line = strtok_r(NULL, delim, &saveptr1);

            endptr = NULL;
            model->normals[norm_i].z = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);
            //printf("%f, %f, %f\n",  model->normals[norm_i].x, model->normals[norm_i].y, model->normals[norm_i].z);
            norm_i++;

        } else if (strcmp(line, "vt") == 0) {
            //Skips char vt
            line = strtok_r(NULL, delim, &saveptr1);

            char *endptr;
            double x = strtod(line, &endptr);
            model->textures[texture_i].x = x;
            
            line = strtok_r(NULL, delim, &saveptr1);
            
            endptr = NULL;
            double y = strtod(line, &endptr);
            model->textures[texture_i].y = y;
            line = strtok_r(NULL, delim, &saveptr1);

            endptr = NULL;
            model->textures[texture_i].z = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);
            //printf("%f, %f, %f\n",  model->textures[texture_i].x, model->textures[texture_i].y, model->textures[texture_i].z);        
            texture_i++;

        }  
         prev_line = strtok_r(buffer, delim, &saveptr1);

    }

    fclose(fptr);
    free(buffer);
    
    model->vertices_size = vert_i;
    model->triangles_size = face_i;
    model->norm_size = norm_i;
    model->texture_size = texture_i;

    
    return model;
}

vector4f normal(Model self, vector2f uv) {
    //(row*height)+column
    double x = uv.x * self.tga_header->width;
    double y = (1-uv.y) * self.tga_header->height;
    //Finds coordinates from uv image
    int i = (y * self.tga_header->height) + x;
    vector4f color;

    //From 2D image
    color = (vector4f){self.uv[i].r, self.uv[i].g, self.uv[i].b,  self.uv[i].a};
    color = subtract_vec4f(scale_vec4f(color, 2./255.), (vector4f){1, 1, 1, 0});
    //printf("%d: %d, %d, %d \n", i, color.x, color.y, color.z);
    return color;
}