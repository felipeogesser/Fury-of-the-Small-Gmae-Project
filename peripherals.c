#include "peripherals.h"
#include <SDL2/SDL.h>

void get_keyboard_input(GameState *game, Player *player, _Bool *window_running) {

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
        
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) *window_running = false;
        if (e.type == SDL_QUIT) *window_running = false;
        
        if (e.type == SDL_KEYUP) {
            switch (e.key.keysym.sym) {
                case SDLK_UP    : game->dirUp    = 0;
                    break;
                case SDLK_DOWN  : game->dirDown  = 0;
                    break;
                case SDLK_LEFT  : game->dirLeft  = 0;
                    break;
                case SDLK_RIGHT : game->dirRight = 0;
                    break;
            }
        }
        
        if (e.type == SDL_KEYDOWN){
            switch (e.key.keysym.sym) {
                case SDLK_UP    : game->dirUp    = -player->speed;
                    break;
                case SDLK_DOWN  : game->dirDown  =  player->speed;
                    break;
                case SDLK_LEFT  : game->dirLeft  = -player->speed;
                    break;
                case SDLK_RIGHT : game->dirRight =  player->speed;
                    break;
            }
        }
    }    
}

void process_peripherals(GameState *game, Player *player, _Bool *window_running) {
    
    get_keyboard_input(game, player, window_running);

}
