#include "image_view.h"
#include "render.h"
#include "model.h"
#include "tga_image.h"




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


int main(int argc, char **argv)
{

    int SCR_WIDTH = 1000;
    int SCR_HEIGHT = 1000;

    int mouse_x = 0;
    int mouse_y = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface *draw_surface = NULL;
    draw_surface = SDL_ScaleSurface(draw_surface, 640, 480, SDL_SCALEMODE_PIXELART);

    bool success = SDL_Init(SDL_INIT_VIDEO);
    if (!success)
    {
        SDL_Log("SDL initialization failed!: %s\n", SDL_GetError());
    }

    SDL_Window *window = SDL_CreateWindow("No Nonsense", SCR_WIDTH, SCR_HEIGHT, 0);
    
    //Mouse Setup
    //SDL_HideCursor();
    SDL_CaptureMouse(true);
    //cursor stays in window
    SDL_SetWindowRelativeMouseMode(window, true);
    //SDL_SetWindowMouseGrab(window, true);

    bool run = true;

    Shader shader;
    shader.camera.position = (vector3f){0, 0, 4};
    shader.camera.direction = (vector3f){0, 0, -1};
    shader.camera.up = (vector3f){0, 1, 0};
    shader.light.direction = (vector3f){0, 0, 1};
    

    shader.ModelView = lookat(shader.camera.position, add_vec3f(shader.camera.direction,shader.camera.position),shader.camera.up);
    shader.Perspective = perspective(norm_vec3f(subtract_vec3f(shader.camera.position,shader.camera.direction)));
    shader.Viewport = viewport(SCR_WIDTH / 16.f, SCR_HEIGHT / 16.f, SCR_WIDTH * 7.f / 8.f, SCR_HEIGHT * 7.f / 8.f);


    int buf_size = SCR_WIDTH * SCR_HEIGHT;
    float depth_buffer[buf_size];
    float zbuffer[buf_size];
    for (int z = 0; z < buf_size; z++)
    {
        zbuffer[z] = -DBL_MAX;
        depth_buffer[z] = -DBL_MAX;
    }

    //Time step setup
    uint32_t current_time = 0;
    uint32_t last_time;
    int FPS = 60;
    int64_t tick_interval = 1000 /FPS;
    float delta_time = 0;

    float move = 0.05f;

    float cam_speed = 0.0;


    image_view color_buffer;
    color_buffer.width = SCR_WIDTH;
    color_buffer.height = SCR_HEIGHT;

    Model cube = load_obj("./src/models/brickwall/cube.obj");
    cube.uv = load_tga("./src/models/brickwall/brickwall_normal.tga", &cube.header_uv);
    cube.diffuse = load_tga("./src/models/brickwall/brickwall_diffuse.tga", &cube.header_diffuse);
    
    Model obj_model  = load_obj("./src/models/diablo/diablo3_pose.obj");
    obj_model.uv = load_tga("./src/models/diablo/diablo3_pose_nm_tangent.tga", &obj_model.header_uv);
    obj_model.diffuse = load_tga("./src/models/diablo/diablo3_pose_diffuse.tga", &obj_model.header_diffuse);
    obj_model.specular = load_tga("./src/models/diablo/diablo3_pose_spec.tga", &obj_model.header_specular);
    

    bool first_mouse = true;
    float yaw   = -90.0f;	
    float pitch =  0.0f;
    float last_x =  800.0f / 2.0;
    float last_y =  600.0 / 2.0;

    mu_Context *ctx = malloc(sizeof(mu_Context));
    mu_init(ctx);
    ctx->text_width = text_width;
    ctx->text_height = text_height;
    bool mouse_down = false;

    while (run)
    {
        
        SDL_Event event;

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
            case SDL_EVENT_MOUSE_MOTION: {
                float x, y;
                Uint32 buttons = SDL_GetMouseState(&x, &y);
                if (!buttons || !SDL_BUTTON_LMASK)
                    continue;
                mu_input_mousemove(ctx, event.motion.x, event.motion.y); 
                float x_pos = event.motion.x;
                float y_pos = event.motion.y; 

                if(first_mouse) {
                    last_x = x_pos;
                    last_y = y_pos;
                    first_mouse = false;
                }

                float x_offset = x_pos - last_x;
                float y_offset = last_y - y_pos;
                last_x = x_pos;
                last_y = y_pos;

                float sensitivity = 0.1f;
                x_offset *= sensitivity;
                y_offset *= sensitivity;
                
                yaw += x_offset;
                pitch += y_offset;

                if (pitch > 89.0f)
                    pitch = 89.0f;
                if (pitch < -89.0f)
                    pitch = -89.0f;

                vector3f front;
                front.x = cos(radian(yaw) * cosf(radian(pitch)));
                front.y = sin(radian(pitch));
                front.z = sin(radian(yaw) * cosf(radian(pitch)));
                shader.camera.direction = (front);
                shader.ModelView = lookat(shader.camera.position, add_vec3f(shader.camera.direction, shader.camera.position), shader.camera.up);

                break;
            }
            case SDL_EVENT_MOUSE_WHEEL: mu_input_scroll(ctx, 0, event.wheel.y * -30); break;
            case SDL_EVENT_TEXT_INPUT: mu_input_text(ctx, event.text.text); break;
            case SDL_EVENT_MOUSE_BUTTON_DOWN: break;
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
                        shader.camera.position = add_vec3f(scale_vec3f(shader.camera.direction, cam_speed), shader.camera.position); 
                        shader.ModelView = lookat(shader.camera.position, add_vec3f(shader.camera.direction, shader.camera.position), shader.camera.up);

                    }
                    if (event.key.key == SDLK_S) {
                        shader.camera.position = subtract_vec3f(shader.camera.position, scale_vec3f(shader.camera.direction, cam_speed));
                        shader.ModelView = lookat(shader.camera.position, add_vec3f(shader.camera.direction, shader.camera.position), shader.camera.up);
                        
                    }
                    if (event.key.key == SDLK_A) {
                        shader.camera.position = subtract_vec3f(shader.camera.position, scale_vec3f(normalize_vec3f(cross(shader.camera.direction, shader.camera.up)), cam_speed));
                        shader.ModelView = lookat(shader.camera.position, add_vec3f(shader.camera.direction, shader.camera.position), shader.camera.up);
                    }
                    if (event.key.key == SDLK_D) {
                        shader.camera.position = add_vec3f(shader.camera.position, scale_vec3f(normalize_vec3f(cross(shader.camera.direction, shader.camera.up)), cam_speed));
                        shader.ModelView = lookat(shader.camera.position, add_vec3f(shader.camera.direction, shader.camera.position), shader.camera.up);
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
        color_buffer.pixels = (color4ub *)draw_surface->pixels;

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
        //vector4f backgroundColor = {0.68f, 0.75f, 0.83f, 1.0f};
        vector4f backgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
        clear(&color_buffer, &backgroundColor);
        color_buffer.at = image_view_at;
        

        //***************************WORLD SCENE RENDERER***************************
        obj_model.angle += radian(90.0f);
        render_faces(&shader, &obj_model, zbuffer, depth_buffer, &color_buffer, false);

        // for (int i = 0; i < 4; i++)
        // {
        //     cube.position = (vector3f){i+1.0f, 0.0f, 0.0f};
        //     render_faces(&shader, &cube, zbuffer, depth_buffer, &color_buffer, false);

        // } 
        for (int z = 0; z < buf_size; z++)
        {
            depth_buffer[z] = -DBL_MAX;
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

                    //render_gui_texture(&color_buffer, dst, src, cmd->text.color);
                    dst.x += dst.w;
                }
            }
            if (cmd->type == MU_COMMAND_RECT) {
                //render_gui(&color_buffer, cmd->rect.rect, atlas[ATLAS_WHITE], cmd->rect.color);

            }
            if (cmd->type == MU_COMMAND_ICON) {
                mu_Rect src = atlas[cmd->icon.id];
                int x = cmd->icon.rect.x + (cmd->icon.rect.w - src.w) / 2;
                int y = cmd->icon.rect.y + (cmd->icon.rect.h - src.h) / 2;

                //render_gui_texture(&color_buffer, mu_rect(x, y, src.w, src.h), src, cmd->icon.color);
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


    free(cube.vertices);
    free(cube.triangles);
    free(cube.normals);
    free(cube.textures);
    free(cube.uv);
    free(cube.diffuse);

    
    free(obj_model.triangles);
    free(obj_model.uv);
    free(obj_model.diffuse);
    free(obj_model.specular);
    free(obj_model.textures);
    free(obj_model.vertices);
    free(obj_model.normals);


    SDL_DestroySurface(draw_surface);
    // SDL_DestroyEvent(event);
    SDL_DestroyWindow(window);
    SDL_Quit();

    

    return 0;
}