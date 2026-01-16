
#include "image_view.h"
#include "render.h"
#include "model.h"






int main(int argc, char **argv)
{

    int SCR_WIDTH = 1000;
    int SCR_HEIGHT = 1000;

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

    Camera *camera = malloc(sizeof(Camera));
    camera->position = (vector3f){0, 0, 3};
    camera->direction = (vector3f){0, 0, -1};
    camera->up = (vector3f){0, 1, 0};

    Light *light = malloc(sizeof(Light));
    light->direction = (vector3f){0, 1, 0};
    

    Shader *obj_model = malloc(sizeof(Shader));
    obj_model->camera = camera;
    obj_model->light = light;
    obj_model->model = read_model_lines("./src/models/african_head.obj");
    obj_model->model->color = (vector4f){255.0f, 255.0f, 255.0f, 255.0f};
    obj_model->model->scale = 0.5f;

    obj_model->ModelView = lookat(camera->position, add_vec3(camera->direction, camera->position), camera->up);
    obj_model->Perspective = perspective(norm(subtract_vec3(camera->position, camera->direction)));
    obj_model->Viewport = viewport(SCR_WIDTH / 16.f, SCR_HEIGHT / 16.f, SCR_WIDTH * 7.f / 8.f, SCR_HEIGHT * 7.f / 8.f);

    Shader *cube = malloc(sizeof(Shader));
    cube->model = malloc(sizeof(Model));
    cube->model->color = (vector4f){75.0f, 255.0f, 75.0f, 255.0f};
    cube->model->scale = 0.2f;
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
    cube->model->vertices = malloc(vert_count * sizeof(vector3f));
    int v_i = 0;
    for (int i = 0; i < 36; i++)
    {

        cube->model->vertices[i].x = vertices[v_i];
        v_i++;
        cube->model->vertices[i].y = vertices[v_i];
        v_i++;
        cube->model->vertices[i].z = vertices[v_i];
        v_i++;
    }

    cube->model->vertices_size = vert_count;
    cube->model->norm_size = vert_count;

    // vert indices
    cube->model->triangles = malloc(36 * sizeof(int));
    for (int i = 0; i < 36; i++)
    {
        cube->model->triangles[i] = i;
    }

    cube->model->triangles_size = 36;
    cube->model->normals = find_normals(cube->model->vertices, cube->model->vertices_size, cube->model->triangles, cube->model->triangles_size);

    cube->ModelView = obj_model->ModelView;
    cube->Perspective = obj_model->ModelView;

    cube->Viewport = obj_model->Viewport;

    int zbuf_size = SCR_WIDTH * SCR_HEIGHT;
    double *zbuffer = malloc(sizeof(double) * zbuf_size);
    for (int z = 0; z < zbuf_size; z++)
    {
        zbuffer[z] = -DBL_MAX;
    }

    Uint64 current_time;
    Uint64 last_time;
    double dt = 0;

    cube->model->angle = 0;
    float move = 0.05f;

    float cam_speed = 0.05f;
    while (run)
    {
        
        SDL_Event event;
        last_time = current_time;
        current_time = SDL_GetPerformanceCounter();

        dt = (double)((current_time - last_time) * 25 / (double)SDL_GetPerformanceFrequency());

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
                    if (event.key.key == SDLK_W) {
                        obj_model->camera->position = add_vec3(scale_vec3(obj_model->camera->direction, cam_speed), obj_model->camera->position); 
                    }
                    if (event.key.key == SDLK_S) {
                        obj_model->camera->position = subtract_vec3(obj_model->camera->position, scale_vec3(obj_model->camera->direction, cam_speed)); 
                    }
                    if (event.key.key == SDLK_A) {
                        obj_model->camera->position = subtract_vec3(obj_model->camera->position, scale_vec3(normalize(cross(obj_model->camera->direction, obj_model->camera->up)), cam_speed));
                    }
                    if (event.key.key == SDLK_D) {
                        obj_model->camera->position = add_vec3(obj_model->camera->position, scale_vec3(normalize(cross(obj_model->camera->direction, obj_model->camera->up)), cam_speed));
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

        // cube->model->angle += dt;

        // // printf("%f \n", dt);
        // if (move > 1.0f)
        //     move = -1.0f;

        // render_faces(cube, zbuffer, &color_buffer, true, 0);
        //Update model view for transforming based on cam changes
        obj_model->ModelView = lookat(obj_model->camera->position, add_vec3(obj_model->camera->direction, obj_model->camera->position), obj_model->camera->up);
        render_faces(obj_model, zbuffer, &color_buffer, true, 0);
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
    free(cube->model->triangles);
    free(cube->model->vertices);
    free(cube->model->normals);

    free(cube->model);
    free(cube);
    free(obj_model->model->triangles);
    free(obj_model->model->textures);
    free(obj_model->model->vertices);
    free(obj_model->model->normals);
    free(obj_model->model);
    free(obj_model->camera);
    free(obj_model->light);
    free(obj_model);


    SDL_DestroySurface(draw_surface);
    // SDL_DestroyEvent(event);
    SDL_DestroyWindow(window);
    SDL_Quit();

    

    return 0;
}