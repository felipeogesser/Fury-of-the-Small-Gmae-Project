#include "peripherals.h"
#include "scene_handler.h"
#include <stdbool.h>
#include <SDL2/SDL.h>

void process_peripherals(_Bool *window_running) {
    
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
            *window_running = false;
            break;
        }
        
        if (e.type == SDL_QUIT) {
            *window_running = false;
            break;
        }
        
        scene_input(&e);

    }

}


/*void get_keyboard_input(GameState *game, Player *player, _Bool *window_running) {

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) *window_running = false;
        if (e.type == SDL_QUIT) *window_running = false;
        
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_UP    : game->dir_up    = 0;
                    break;
                case SDLK_DOWN  : game->dir_down  = 0;
                    break;
                case SDLK_LEFT  : game->dir_left  = 0;
                    break;
                case SDLK_RIGHT : game->dir_right = 0;
                    break;
            }
        }
        
        if (e.type == SDL_KEYDOWN){
            switch (e.key.keysym.sym) {
                case SDLK_UP    : game->dir_up    = -player->speed;
                    break;
                case SDLK_DOWN  : game->dir_down  =  player->speed;
                    break;
                case SDLK_LEFT  : game->dir_left  = -player->speed;
                    break;
                case SDLK_RIGHT : game->dir_right =  player->speed;
                    break;
            }
        }
    }    
}*/