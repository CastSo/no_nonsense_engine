#include "image_view.h"
#include "render.h"
#include "model.h"


int main(int argc, char **argv)
{

    int SCR_WIDTH = 800;
    int SCR_HEIGHT = 800;

    int mouse_x = 0;
    int mouse_y = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *draw_surface = NULL;

    bool success = SDL_Init(SDL_INIT_VIDEO);
    if (!success)
    {
        SDL_Log("SDL initialization failed!: %s\n", SDL_GetError());
    }

    SDL_Window *window = SDL_CreateWindow("Tiny Renderer", SCR_WIDTH, SCR_HEIGHT, 0);
    

    bool run = true;

    Shader *shader = malloc(sizeof(Shader));
    shader->camera = malloc(sizeof(Camera));
    shader->camera->position = (vector3f){0, 0, 3};
    shader->camera->direction = (vector3f){0, 0, -1};
    shader->camera->up = (vector3f){0, 1, 0};
    shader->light = malloc(sizeof(Light));
    shader->light->direction = (vector3f){0, 1, 0};
    

    Model *obj_model  = read_model_lines("./src/models/diablo3_pose.obj");
    obj_model->color = (vector4f){255.0f, 255.0f, 255.0f, 255.0f};
    obj_model->scale = 0.5f;

    shader->ModelView = lookat(shader->camera->position, add_vec3(shader->camera->direction,shader->camera->position),shader->camera->up);
    shader->Perspective = perspective(norm(subtract_vec3(shader->camera->position,shader->camera->direction)));
    shader->Viewport = viewport(SCR_WIDTH / 16.f, SCR_HEIGHT / 16.f, SCR_WIDTH * 7.f / 8.f, SCR_HEIGHT * 7.f / 8.f);

    Model *cube = malloc(sizeof(Model));
    cube->color = (vector4f){75.0f, 255.0f, 75.0f, 255.0f};
    cube->scale = 0.2f;
    int vert_count = 36;
    double vertices[108] = {
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,

        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,

        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,

        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        -0.5f, -0.5f, -0.5f,

        -0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, -0.5f,
        0.5f, 0.5f, 0.5f,
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, -0.5f};
    cube->vertices = malloc(vert_count * sizeof(vector3f));
    int v_i = 0;
    for (int i = 0; i < 36; i++)
    {

        cube->vertices[i].x = vertices[v_i];
        v_i++;
        cube->vertices[i].y = vertices[v_i];
        v_i++;
        cube->vertices[i].z = vertices[v_i];
        v_i++;
    }

    cube->vertices_size = vert_count;
    cube->norm_size = vert_count;

    // vert indices
    cube->triangles = malloc(36 * sizeof(int));
    for (int i = 0; i < 36; i++)
    {
        cube->triangles[i] = i;
    }

    cube->triangles_size = 36;
    cube->normals = find_normals(cube->vertices, cube->vertices_size, cube->triangles, cube->triangles_size);


    int zbuf_size = SCR_WIDTH * SCR_HEIGHT;
    double *zbuffer = malloc(sizeof(double) * zbuf_size);
    for (int z = 0; z < zbuf_size; z++)
    {
        zbuffer[z] = -DBL_MAX;
    }

    Uint64 current_time;
    Uint64 last_time;
    double dt = 0;

    cube->angle = 90;
    float move = 0.05f;

    float cam_speed = 0.0;
    while (run)
    {
        
        SDL_Event event;
        last_time = current_time;
        current_time = SDL_GetPerformanceCounter();

        
        dt = (double)((current_time - last_time) * 25 / (double)SDL_GetPerformanceFrequency());
        cam_speed = 2.5f * dt;

        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {

            case SDL_EVENT_WINDOW_RESIZED:
                if (draw_surface)
                {
                    SDL_DestroySurface(draw_surface);
                }
                draw_surface = NULL;

                SCR_WIDTH = event.window.data1;
                SCR_HEIGHT = event.window.data2;
                break;

            case SDL_EVENT_QUIT:
                run = false;
                break;
            case SDL_EVENT_MOUSE_MOTION:
                mouse_x = event.motion.x;
                mouse_y = event.motion.y;
                break;
            case SDL_EVENT_KEY_DOWN:
                switch (event.type)
                {
                case SDL_EVENT_KEY_DOWN:
                    if (event.key.key == SDLK_ESCAPE)
                    {

                        run = false;
                        break;
                    }
                    //Update cam position and game objects modelview
                    if (event.key.key == SDLK_W) {
                        shader->camera->position = add_vec3(scale_vec3(shader->camera->direction, cam_speed), shader->camera->position); 
                        shader->ModelView = lookat(shader->camera->position, add_vec3(shader->camera->direction, shader->camera->position), shader->camera->up);
                    }
                    if (event.key.key == SDLK_S) {
                        shader->camera->position = subtract_vec3(shader->camera->position, scale_vec3(shader->camera->direction, cam_speed)); 
                        shader->ModelView = lookat(shader->camera->position, add_vec3(shader->camera->direction, shader->camera->position), shader->camera->up);
                    }
                    if (event.key.key == SDLK_A) {
                        shader->camera->position = subtract_vec3(shader->camera->position, scale_vec3(normalize(cross(shader->camera->direction, shader->camera->up)), cam_speed));
                        shader->ModelView = lookat(shader->camera->position, add_vec3(shader->camera->direction, shader->camera->position), shader->camera->up);
                    }
                    if (event.key.key == SDLK_D) {
                        shader->camera->position = add_vec3(shader->camera->position, scale_vec3(normalize(cross(shader->camera->direction, shader->camera->up)), cam_speed));
                        shader->ModelView = lookat(shader->camera->position, add_vec3(shader->camera->direction, shader->camera->position), shader->camera->up);
                    }


                }
                break;
            }
        }

        if (!run)
            break;

        //***************************WORLD SCENE RENDERER***************************
        if (!draw_surface)
        {
            draw_surface = SDL_CreateSurface(SCR_WIDTH, SCR_HEIGHT, SDL_PIXELFORMAT_RGBA32);
            SDL_SetSurfaceBlendMode(draw_surface, SDL_BLENDMODE_NONE);
        }

        // Sets color buffer for screen
        image_view color_buffer;
        color_buffer.pixels = (color4ub *)draw_surface->pixels,
        color_buffer.width = SCR_WIDTH;
        color_buffer.height = SCR_HEIGHT;
        vector4f backgroundColor = {0.7f, 1.0f, 1.0f, 1.0f};

        // Set background color
        clear(&color_buffer, &backgroundColor);

        color_buffer.at = image_view_at;

        cube->angle += dt;


        //Update model view for transforming based on cam changes
        render_faces(shader, obj_model, zbuffer, &color_buffer, true, 0);
        for (int z = 0; z < zbuf_size; z++)
        {
            zbuffer[z] = -DBL_MAX;
        }


        render_faces(shader, cube, zbuffer, &color_buffer, false, 0);
        // Reset the zbuffer
        for (int z = 0; z < zbuf_size; z++)
        {
            zbuffer[z] = -DBL_MAX;
        }

        SDL_Rect draw_rect;
        draw_rect.x = 0;
        draw_rect.y = 0;
        draw_rect.w = SCR_WIDTH;
        draw_rect.h = SCR_HEIGHT;
        SDL_BlitSurface(draw_surface, &draw_rect,
                        SDL_GetWindowSurface(window), &draw_rect);

        SDL_UpdateWindowSurface(window);




    }

    free(zbuffer);
    free(shader->camera);
    free(shader->light);
    free(shader);

    free(cube->triangles);
    free(cube->vertices);
    free(cube->normals);
    free(cube);
    
    free(obj_model->triangles);
    free(obj_model->textures);
    free(obj_model->vertices);
    free(obj_model->normals);
    free(obj_model);
    


    SDL_DestroySurface(draw_surface);
    // SDL_DestroyEvent(event);
    SDL_DestroyWindow(window);
    SDL_Quit();

    

    return 0;
}