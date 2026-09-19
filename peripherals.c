#include "peripherals.h"
#include "scene_handler.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

void process_peripherals(_Bool *window_running) {
    
    SDL_Event e;
    signed int queue_not_empty = SDL_PollEvent(&e);

    if (!queue_not_empty) {

        SDL_Event e_fake = {0};
        scene_input(&e_fake);

    }

    while (queue_not_empty) {
        
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            *window_running = false;
            continue;
        }
        
        if (e.type == SDL_QUIT) {
            *window_running = false;
            break;
        }
        
        scene_input(&e);

        queue_not_empty = SDL_PollEvent(&e);

    }

}
