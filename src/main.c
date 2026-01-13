#include "image_view.h"
#include "render.h"
#include "model.h"
#include "nuklear.h"
// #include "nuklear_sdl3_renderer.h"
// #include "nuklear_def.h"

   
int main(int argc, char** argv) {
    int SCR_WIDTH = 1000;
    int SCR_HEIGHT = 1000;

    int mouse_x = 0;
	int mouse_y = 0;

    SDL_Init(SDL_INIT_VIDEO);
    SDL_Surface * draw_surface = NULL; 

    bool success = SDL_Init(SDL_INIT_VIDEO);
    if( !success ) {
        SDL_Log("SDL initialization failed!: %s\n", SDL_GetError());
    }



    SDL_Window* window = SDL_CreateWindow("Tiny Renderer",
                                        SCR_WIDTH, SCR_HEIGHT,
                                        0);

    bool run = true;
    
    vector3f cam_pos = {0, 0, 6};
    vector3f cam_direction = {0, 0, 0};
    vector3f cam_up = {0, 1, 0};
    
    Shader *obj_model = malloc(sizeof(Shader));
    obj_model->model = read_model_lines("./src/models/african_head.obj");
    obj_model->model->color = (vector4f) {255.0f, 255.0f, 255.0f, 255.0f};
    obj_model->model->scale = 0.5f;

    obj_model->ModelView = lookat(cam_pos, cam_direction, cam_up);
    obj_model->Perspective = perspective(norm(subtract_vec3(cam_pos, cam_direction)));
    obj_model->Viewport = viewport(SCR_WIDTH/16.f, SCR_HEIGHT/16.f, SCR_WIDTH*7.f/8.f, SCR_HEIGHT*7.f/8.f);


    Shader *cube = malloc(sizeof(Shader));
    cube->model = malloc(sizeof(Model));
    cube->model->color = (vector4f) {75.0f, 255.0f, 75.0f, 255.0f};
    cube->model->scale = 0.2f;
    int vert_count = 36;
    double vertices[108] ={
        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  

        -0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  

        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f, -0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f, -0.5f,  

        -0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f, -0.5f
    };
    cube->model->vertices = malloc(vert_count * sizeof(vector3f));
    int v_i = 0;
    for(int i = 0; i < 36; i++ ) {
        
        cube->model->vertices[i].x = vertices[v_i];
        v_i++;
        cube->model->vertices[i].y = vertices[v_i];
        v_i++;
        cube->model->vertices[i].z = vertices[v_i];
        v_i++;

    }


     cube->model->vertices_size = vert_count;
    cube->model->norm_size = vert_count;

    //vert indices
    cube->model->triangles = malloc(36 * sizeof(int));
    for(int i = 0; i < 36; i++) {
        cube->model->triangles[i] = i;
    }

    
    cube->model->triangles_size = 36;
    cube->model->normals = find_normals(cube->model->vertices, cube->model->vertices_size, cube->model->triangles, cube->model->triangles_size);

    cube->ModelView = lookat(cam_pos, cam_direction, cam_up);
    cube->Perspective = perspective(norm(subtract_vec3(cam_pos, cam_direction)));

    cube->Viewport = viewport(SCR_WIDTH/16.f, SCR_HEIGHT/16.f, SCR_WIDTH*7.f/8.f, SCR_HEIGHT*7.f/8.f);


    int zbuf_size = SCR_WIDTH * SCR_HEIGHT;
    double *zbuffer = malloc(sizeof(double) * zbuf_size);
    for (int z = 0; z < zbuf_size; z++) {
        zbuffer[z] = -DBL_MAX;
    }

    Uint64 current_time ;
    Uint64 last_time;
    double dt = 0;

    cube->model->angle =0;
    float move = 0.05f;

    // init gui state
    enum {EASY, HARD};
    static int op = EASY;
    static float value = 0.6f;
    static int i =  20;
    struct nk_context *ctx;
    

   
    // nk_end(&ctx);
    //     if (nk_begin(ctx, "Demo", nk_rect(50, 50, 230, 250),
    //     NK_WINDOW_BORDER|NK_WINDOW_MOVABLE|NK_WINDOW_SCALABLE|
    //     NK_WINDOW_MINIMIZABLE|NK_WINDOW_TITLE)) {

    //     }

    while (run) {
        
        SDL_Event event;
        last_time = current_time;
        current_time = SDL_GetPerformanceCounter();
        
        dt = (double)((current_time - last_time)*25 / (double)SDL_GetPerformanceFrequency());

        while(SDL_PollEvent(&event)) {
            switch (event.type){

            case SDL_EVENT_WINDOW_RESIZED:
                if(draw_surface) {
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
                switch(event.type) {
                case SDL_EVENT_KEY_DOWN: 
                    if(event.key.key == SDLK_ESCAPE)
                    {                       
                        run = false;
                        break;
                    }
                }
                break;
            
        }
        }

        if (!run)
            break;


        if (!draw_surface) {
            draw_surface = SDL_CreateSurface( SCR_WIDTH, SCR_HEIGHT, SDL_PIXELFORMAT_RGBA32);
            SDL_SetSurfaceBlendMode(draw_surface, SDL_BLENDMODE_NONE);
        }

		
        image_view color_buffer;
        color_buffer.pixels = (color4ub *)draw_surface->pixels,
        color_buffer.width = SCR_WIDTH;
        color_buffer.height = SCR_HEIGHT;
        vector4f backgroundColor = {0.86f, 0.78f, 0.56f, 1.0f};
        
        color_buffer.at = image_view_at;

        //Set background color
        clear(&color_buffer, &backgroundColor);

        cube->model->angle += dt;

        //printf("%f \n", dt);
        if(move > 1.0f) 
            move = -1.0f;


        // render_faces(obj_model, zbuffer, &color_buffer, 0);
        // //Reset the zbuffer
        // for (int z = 0; z < zbuf_size; z++) {
        //     zbuffer[z] = -DBL_MAX;
        // }

        render_faces(cube, zbuffer, &color_buffer, true, 0);
        //Reset the zbuffer
        for (int z = 0; z < zbuf_size; z++) {
            zbuffer[z] = -DBL_MAX;
        }

        //render_wireframe(shader->model, &color_buffer);



        SDL_Rect rect;
        rect.x = 0; 
        rect.y = 0; 
        rect.w = SCR_WIDTH; 
        rect.h = SCR_HEIGHT;
        SDL_BlitSurface(draw_surface, &rect,
            SDL_GetWindowSurface(window), &rect);

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
    free(obj_model);


    SDL_DestroySurface(draw_surface);
    //SDL_DestroyEvent(event);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}