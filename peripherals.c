#include "peripherals.h"
#include "scene_handler.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

void process_peripherals(_Bool *window_running) {
    
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            *window_running = false;
            continue;
        }
        
        if (e.type == SDL_QUIT) {
            *window_running = false;
            break;
        }
        
        scene_input(&e);
    }

}
