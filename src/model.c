#include "model.h"


struct Model* read_model_lines(char *file_name) {
    FILE *fptr = fopen(file_name, "r");
    if (fptr == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    Model *model = malloc(sizeof(Model));

    int vertices_size;
    model->vertices = malloc(1000000 * sizeof(vector3f));
    int triangles_size;
    model->triangles = malloc(1000000 * sizeof(int));
    int normals_size;
    model->normals = malloc(1000000 * sizeof(vector3f));
    int texture_size;
    model->textures = malloc(1000000 * sizeof(vector3f));


    char *buffer;
    size_t buffer_size = 256;
    size_t characters;

    char *line = NULL;
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
            
            //Only checks the first index of each word
            while (line != NULL) {      
                //Skips char f
                char *current_vert = strtok_r(line, "", &saveptr2);

                if(atoi(current_vert) != 0) {
                    model->triangles[face_i] = atoi(current_vert);
                    // printf("%d\n",  model->triangles[face_i]);
                    face_i++;
                }
                
                line = strtok_r(NULL, delim, &saveptr1);

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

            norm_i++;

        } else if (strcmp(line, "vt") == 0) {
            //Skips char vt
            line = strtok_r(NULL, delim, &saveptr1);

            char *endptr;
            double x = strtod(line, &endptr);
            model->textures[norm_i].x = x;
            
            line = strtok_r(NULL, delim, &saveptr1);
            
            endptr = NULL;
            double y = strtod(line, &endptr);
            model->textures[norm_i].y = y;
            line = strtok_r(NULL, delim, &saveptr1);

            endptr = NULL;
            model->textures[norm_i].z = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);
                    
            texture_i++;

        }  


    }

    fclose(fptr);
    free(buffer);
    
    model->vertices_size = vert_i;
    model->triangles_size = face_i;
    model->norm_size = norm_i;
    model->texture_size = texture_i;

    return model;
}

// vector4f normal(vector2f uv, int width, int height) {
//     vector2f c = {uv.x * width, uv.y * height};
// }