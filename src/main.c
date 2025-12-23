#include "type.h"
#include "image_view.h"
#include "render.h"

int main(int argc, char** argv) {
    int SCR_WIDTH = 1920;
    int SCR_HEIGHT = 1080;

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
    Model *model = read_model_lines("src/models/african_head.obj");


    while (run) {
        SDL_Event event;

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
        vector4f backgroundColor = {0.0f, 0.0f, 0.0f, 1.0f};
        
        color_buffer.at = image_view_at;

        //Set background color
        clear(&color_buffer, &backgroundColor);

        render_wireframe(model, &color_buffer);

        SDL_Rect rect;
        rect.x = 0; 
        rect.y = 0; 
        rect.w = SCR_WIDTH; 
        rect.h = SCR_HEIGHT;
        SDL_BlitSurface(draw_surface, &rect,
            SDL_GetWindowSurface(window), &rect);

        SDL_UpdateWindowSurface(window);
    }
    free(model->vertices);
    free(model->triangles);
    free(model);

    SDL_DestroySurface(draw_surface);
    //SDL_DestroyEvent(event);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}