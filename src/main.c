#include "image_view.h"
#include "render.h"
#include "model.h"
#include "tga_image.h"


static  char logbuf[64000];
static   int logbuf_updated = 0;

static const char button_map[256] = {
  [ SDL_BUTTON_LEFT   & 0xff ] =  MU_MOUSE_LEFT,
  [ SDL_BUTTON_RIGHT  & 0xff ] =  MU_MOUSE_RIGHT,
  [ SDL_BUTTON_MIDDLE & 0xff ] =  MU_MOUSE_MIDDLE,
};

int text_width(mu_Font font, const char *text, int len) {
    int res = 0;
    for (const char *p = text; *p && len--; p++) {
        if ((*p & 0xc0) == 0x80) { continue; }
            int chr = mu_min((unsigned char) *p, 127);
            res += atlas[ATLAS_FONT + chr].w;
        }
    return res;
}

int text_height(mu_Font font) {
    return 18;
}

static void write_log(const char *text) {
  if (logbuf[0]) { strcat(logbuf, "\n"); }
  strcat(logbuf, text);
  logbuf_updated = 1;
}




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

    SDL_Window *window = SDL_CreateWindow("No Nonsense", SCR_WIDTH, SCR_HEIGHT, 0);
    

    bool run = true;

    Shader *shader = malloc(sizeof(Shader));
    shader->camera = malloc(sizeof(Camera));
    shader->camera->position = (vector3f){0, 0, 2};
    shader->camera->direction = (vector3f){0, 0, -1};
    shader->camera->up = (vector3f){0, 1, 0};
    shader->light = malloc(sizeof(Light));
    shader->light->direction = (vector3f){1, 1, 1};
    

    Model *obj_model  = read_model_lines("./src/models/diablo3_pose.obj");
    obj_model->color = (vector4f){255.0f, 255.0f, 255.0f, 255.0f};
    //obj_model->scale = 0.5f;

    shader->ModelView = lookat(shader->camera->position, add_vec3f(shader->camera->direction,shader->camera->position),shader->camera->up);
    shader->Perspective = perspective(norm_vec3f(subtract_vec3f(shader->camera->position,shader->camera->direction)));
    shader->Viewport = viewport(SCR_WIDTH / 16.f, SCR_HEIGHT / 16.f, SCR_WIDTH * 7.f / 8.f, SCR_HEIGHT * 7.f / 8.f);

    Model *cube = malloc(sizeof(Model));
    cube->color = (vector4f){75.0f, 255.0f, 75.0f, 255.0f};
    //cube->scale = 0.2f;
    int vert_count = 24;

    //Uses counter-clockwise winding like OpenGL
    float vertices[72] = {
        -0.5f, -0.5f, -0.5f,         // A 0
        0.5f, -0.5f, -0.5f,         // B 1
        0.5f,  0.5f, -0.5f,         // C 2
        -0.5f,  0.5f, -0.5f,         // D 3
        -0.5f, -0.5f,  0.5f,         // E 4
        0.5f, -0.5f,  0.5f,          // F 5
        0.5f,  0.5f,  0.5f,          // G 6
        -0.5f,  0.5f,  0.5f,          // H 7
        
        -0.5f,  0.5f, -0.5f,      // D 8
        -0.5f, -0.5f, -0.5f,         // A 9
        -0.5f, -0.5f,  0.5f,         // E 10
        -0.5f,  0.5f,  0.5f,         // H 11
        0.5f, -0.5f, -0.5f,          // B 12
        0.5f,  0.5f, -0.5f,          // C 13
        0.5f,  0.5f,  0.5f,          // G 14
        0.5f, -0.5f,  0.5f,          // F 15
        
        -0.5f, -0.5f, -0.5f,      // A 16
        0.5f, -0.5f, -0.5f,          // B 17
        0.5f, -0.5f,  0.5f,          // F 18
        -0.5f, -0.5f,  0.5f,         // E 19
        0.5f,  0.5f, -0.5f,          // C 20
        -0.5f,  0.5f, -0.5f,         // D 21
        -0.5f,  0.5f,  0.5f,         // H 22
        0.5f,  0.5f,  0.5f,          // G 23
    };

    cube->triangles_size = 36;
    int cube_indices[36] = {
        // front and back
        0, 3, 2,
        2, 1, 0,
        4, 5, 6,
        6, 7 ,4,
        // left and right
        11, 8, 9,
        9, 10, 11,
        12, 13, 14,
        14, 15, 12,
        // bottom and top
        16, 17, 18,
        18, 19, 16,
        20, 21, 22,
        22, 23, 20
    };
    cube->vertices = malloc(vert_count * sizeof(vector3f));
    int v_i = 0;
    for (int i = 0; i < 24; i++)
    {

        cube->vertices[i].x = vertices[v_i];
        v_i++;
        cube->vertices[i].y = vertices[v_i];
        v_i++;
        cube->vertices[i].z = vertices[v_i];
        v_i++;
    }

    cube->vertices_size = vert_count;

    // vert indices
    cube->triangles = cube_indices;

    cube->normals = find_normals(cube->vertices, cube->vertices_size, cube->triangles, cube->triangles_size);
    cube->norm_size = cube->triangles_size;


    int zbuf_size = SCR_WIDTH * SCR_HEIGHT;
    float *zbuffer = malloc(sizeof(float) * zbuf_size);
    for (int z = 0; z < zbuf_size; z++)
    {
        zbuffer[z] = -DBL_MAX;
    }

    Uint64 current_time = SDL_GetPerformanceCounter();
    Uint64 last_time;
    float dt = 0;

    cube->angle = 60;
    float move = 0.05f;

    float cam_speed = 0.0;


    image_view *color_buffer = malloc(sizeof(image_view));
    color_buffer->width = SCR_WIDTH;
    color_buffer->height = SCR_HEIGHT;
    
    obj_model->header_uv = malloc(sizeof(TGAHeader));
    obj_model->header_diffuse = malloc(sizeof(TGAHeader));
    obj_model->header_specular = malloc(sizeof(TGAHeader));
    obj_model->uv = load_tga("./src/models/diablo3_pose_nm_tangent.tga", obj_model->header_uv);
    obj_model->diffuse = load_tga("./src/models/diablo3_pose_diffuse.tga", obj_model->header_diffuse);
    obj_model->specular = load_tga("./src/models/diablo3_pose_spec.tga", obj_model->header_specular);
    
    bool first_mouse = true;
    float yaw   = -90.0f;	// yaw is initialized to -90.0 degrees since a yaw of 0.0 results in a direction vector pointing to the right so we initially rotate a bit to the left.
    float pitch =  0.0f;
    float last_x =  800.0f / 2.0;
    float last_y =  600.0 / 2.0;

    mu_Context *ctx = malloc(sizeof(mu_Context));
    mu_init(ctx);
    ctx->text_width = text_width;
    ctx->text_height = text_height;


    while (run)
    {
        
        SDL_Event event;
        last_time = current_time;
        current_time = SDL_GetPerformanceCounter();

        
        dt = (float)((current_time - last_time) * 10000 / (float)SDL_GetPerformanceFrequency());
        cam_speed = 0.8f;

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
            case SDL_EVENT_MOUSE_MOTION: mu_input_mousemove(ctx, event.motion.x, event.motion.y); break;
            case SDL_EVENT_MOUSE_WHEEL: mu_input_scroll(ctx, 0, event.wheel.y * -30); break;
            case SDL_EVENT_TEXT_INPUT: mu_input_text(ctx, event.text.text); break;

            case SDL_EVENT_MOUSE_BUTTON_DOWN:
            case SDL_EVENT_MOUSE_BUTTON_UP: {
                int b = button_map[event.button.button & 0xff];
                if (b && event.type == SDL_EVENT_MOUSE_BUTTON_DOWN) { mu_input_mousedown(ctx, event.button.x, event.button.y, b); }
                if (b && event.type ==   SDL_EVENT_MOUSE_BUTTON_UP) { mu_input_mouseup(ctx, event.button.x, event.button.y, b);   }
                break;
            }
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
                        shader->camera->position = add_vec3f(scale_vec3f(shader->camera->direction, cam_speed), shader->camera->position); 
                        shader->ModelView = lookat(shader->camera->position, add_vec3f(shader->camera->direction, shader->camera->position), shader->camera->up);

                    }
                    if (event.key.key == SDLK_S) {
                        shader->camera->position = subtract_vec3f(shader->camera->position, scale_vec3f(shader->camera->direction, cam_speed));
                        shader->ModelView = lookat(shader->camera->position, add_vec3f(shader->camera->direction, shader->camera->position), shader->camera->up);
                        
                    }
                    if (event.key.key == SDLK_A) {
                        shader->camera->position = subtract_vec3f(shader->camera->position, scale_vec3f(normalize_vec3f(cross(shader->camera->direction, shader->camera->up)), cam_speed));
                        shader->ModelView = lookat(shader->camera->position, add_vec3f(shader->camera->direction, shader->camera->position), shader->camera->up);
                    }
                    if (event.key.key == SDLK_D) {
                        shader->camera->position = add_vec3f(shader->camera->position, scale_vec3f(normalize_vec3f(cross(shader->camera->direction, shader->camera->up)), cam_speed));
                        shader->ModelView = lookat(shader->camera->position, add_vec3f(shader->camera->direction, shader->camera->position), shader->camera->up);
                    }


                }
                break;

            }    
        }

        if (!run)
            break;
            
        if (!draw_surface)
        {
            draw_surface = SDL_CreateSurface(SCR_WIDTH, SCR_HEIGHT, SDL_PIXELFORMAT_RGBA32);
            SDL_SetSurfaceBlendMode(draw_surface, SDL_BLENDMODE_NONE);
        }

        // Sets color buffer for screen
        color_buffer->pixels = (color4ub *)draw_surface->pixels;

        mu_begin(ctx);
        if (mu_begin_window(ctx, "Transformations", mu_rect(40, 40, 240, 300)))  {
            mu_Container *win = mu_get_current_container(ctx);
            win->rect.w = mu_max(win->rect.w, 240);
            win->rect.h = mu_max(win->rect.h, 300);

                /* window info */
            if (mu_header(ctx, "Window Info")) {
                mu_Container *win = mu_get_current_container(ctx);
                char buf[64];
                mu_layout_row(ctx, 2, (int[]) { 54, -1 }, 0);
                mu_label(ctx,"Position:");
                sprintf(buf, "%d, %d", win->rect.x, win->rect.y); 
                mu_label(ctx, buf);
             
            }


            mu_end_window(ctx);
        }
        mu_end(ctx);


        // Set background color
        vector4f backgroundColor = {0.68f, 0.75f, 0.83f, 1.0f};
        clear(color_buffer, &backgroundColor);
        color_buffer->at = image_view_at;
        

        //***************************WORLD SCENE RENDERER***************************

        render_faces(shader, obj_model, zbuffer, color_buffer, true, 0);
        for (int z = 0; z < zbuf_size; z++)
        {
            zbuffer[z] = -DBL_MAX;
        }



        SDL_Rect draw_rect;
        draw_rect.x = 0;
        draw_rect.y = 0;
        draw_rect.w = SCR_WIDTH;
        draw_rect.h = SCR_HEIGHT;


        
        //***************************UI RENDERER***************************
        mu_Command *cmd = NULL;
        
        while (mu_next_command(ctx, &cmd)) {
            if (cmd->type == MU_COMMAND_TEXT) {
                mu_Rect dst = {cmd->text.pos.x, cmd->text.pos.y, 0, 0};

                for (const char *p = cmd->text.str; *p; p++) {
                    if((*p & 0xc0) == 0x80){
                        continue;
                    }
                    //printf("%s", p);
                    int chr = mu_min((unsigned char) *p, 127);
                    mu_Rect src = atlas[ATLAS_FONT + chr];
                    dst.w = src.w;
                    dst.h = src.h;

                    //render_gui_texture(color_buffer, dst, src, cmd->text.color);
                    dst.x += dst.w;
                }
            }
            if (cmd->type == MU_COMMAND_RECT) {
                //render_gui(color_buffer, cmd->rect.rect, atlas[ATLAS_WHITE], cmd->rect.color);

            }
            if (cmd->type == MU_COMMAND_ICON) {
                mu_Rect src = atlas[cmd->icon.id];
                int x = cmd->icon.rect.x + (cmd->icon.rect.w - src.w) / 2;
                int y = cmd->icon.rect.y + (cmd->icon.rect.h - src.h) / 2;

                //render_gui_texture(color_buffer, mu_rect(x, y, src.w, src.h), src, cmd->icon.color);
            }
            if (cmd->type == MU_COMMAND_CLIP) {
                SDL_Rect clip = {
                    cmd->clip.rect.x,
                    cmd->clip.rect.y,
                    cmd->clip.rect.w,
                    cmd->clip.rect.h
                };

                if (cmd->clip.rect.w > 0) {
                    SDL_SetSurfaceClipRect(draw_surface, &clip);
                } else {
                    SDL_SetSurfaceClipRect(draw_surface, NULL);
                }


            }
        }
        
        //Update surface should be done end of an iteration
        SDL_BlitSurface(draw_surface, &draw_rect,
                    SDL_GetWindowSurface(window), &draw_rect);
        SDL_UpdateWindowSurface(window);

    }
    free(ctx);

    free(color_buffer);

    free(zbuffer);
    free(shader->camera);
    free(shader->light);
    free(shader);


    free(cube->vertices);
    free(cube->normals);
    free(cube);
    
    free(obj_model->triangles);
    free(obj_model->uv);
    free(obj_model->diffuse);
    free(obj_model->specular);
    free(obj_model->header_diffuse);
    free(obj_model->header_uv);
    free(obj_model->header_specular);
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