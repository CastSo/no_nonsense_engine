#include "model.h"


struct Model read_model_lines(char *file_name) {
    FILE *fptr = fopen(file_name, "r");
    if (fptr == NULL) {
        perror("Unable to open file");
        exit(1);
    }

    Model model;

    int vertices_size;
    model.vertices = malloc(50000 * sizeof(vector3f));
    int triangles_size;
    model.triangles = malloc(50000 * sizeof(int));
    int normals_size;
    model.normals = malloc(50000 * sizeof(vector3f));
    int texture_size;
    model.textures = malloc(50000 * sizeof(vector3f));


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
            float x = strtod(line, &endptr);
            model.vertices[vert_i].x = x;
            
            line = strtok_r(NULL, delim, &saveptr1);
            
            endptr = NULL;
            float y = strtod(line, &endptr);
            model.vertices[vert_i].y = y;
            line = strtok_r(NULL, delim, &saveptr1);

            endptr = NULL;
            model.vertices[vert_i].z = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);
            //printf("%d, %d, %d\n", model.vertices[vert_i].x, model.vertices[vert_i].y, model.vertices[vert_i].z);
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

                //Loads v, vn, and vt
                while (vert != NULL){
                    int v = atoi(vert);
                    if(v != 0) {
                        //obj starts at 1 so convert to index starting at 0
                        model.triangles[face_i] = v-1;
                        //printf("%d, ", model.triangles[face_i]);
                        face_i++;
                    }
                    vert = strtok_r(NULL, "/", &saveptr2);
                }
                //printf("\n");
                
                prev_line = strtok_r(NULL, " ", &saveptr1);

            }



        } else if (strcmp(line, "vn") == 0) {
            //Skips char vn
            line = strtok_r(NULL, delim, &saveptr1);

            char *endptr;
            float x = strtod(line, &endptr);
            model.normals[norm_i].x = x;
            
            line = strtok_r(NULL, delim, &saveptr1);
            
            endptr = NULL;
            float y = strtod(line, &endptr);
            model.normals[norm_i].y = y;
            line = strtok_r(NULL, delim, &saveptr1);

            endptr = NULL;
            model.normals[norm_i].z = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);
            //printf("%f, %f, %f\n",  model.normals[norm_i].x, model.normals[norm_i].y, model.normals[norm_i].z);
            norm_i++;

        } else if (strcmp(line, "vt") == 0) {
            //Skips char vt
            line = strtok_r(NULL, delim, &saveptr1);

            char *endptr;
            float x = strtod(line, &endptr);
            model.textures[texture_i].x = x;
            
            line = strtok_r(NULL, delim, &saveptr1);
            
            endptr = NULL;
            float y = strtod(line, &endptr);
            model.textures[texture_i].y = y;
            line = strtok_r(NULL, delim, &saveptr1);

            endptr = NULL;
            model.textures[texture_i].z = strtod(line, &endptr);
            line = strtok_r(NULL, delim, &saveptr1);
            //printf("%f, %f, %f\n",  model.textures[texture_i].x, model.textures[texture_i].y, model.textures[texture_i].z);        
            texture_i++;

        }  
         prev_line = strtok_r(buffer, delim, &saveptr1);

    }

    fclose(fptr);
    free(buffer);
    
    model.vertices_size = vert_i;
    model.triangles_size = face_i;
    model.norm_size = norm_i;
    model.texture_size = texture_i;

    
    return model;
}

vector4f normal(TGAHeader *tga_header, color4ub *image, vector2f uv) {
    //normal to screen space
    int x = (int)(uv.x * tga_header->width);
    int y = (int)((1-uv.y) * tga_header->height);

    //Fragment samples from image
    int i = (x + y * tga_header->width);
    vector4f color;
    //printf("%f, %f, %d \n", uv.x, uv.y, self.tga_header->width);

    //From 2D image superimpose 3D model
    color = (vector4f){image[i].r, image[i].g, image[i].b,  0};
    color = subtract_vec4f(scale_vec4f(color, 2./255.), (vector4f){1, 1, 1, 0});

    //printf("%d: %d, %d, %d \n", i, color.x, color.y, color.z);
    return color;
}

//Returns just the pixel RGB
color4ub sample2D(TGAHeader *tga_header, color4ub *image, vector2f uv) {
    //normal to screen space
    int x = (int)(uv.x * tga_header->width);
    int y = (int)((1-uv.y) * tga_header->height);

    //Fragment samples from image
    int i = (x + y * tga_header->width);
    vector4f color;

    return (color4ub){image[i].r, image[i].g, image[i].b,  0};
}