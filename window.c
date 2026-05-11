#include "window.h"
#include "engine.h"

void create_window() {
        
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        return;
    }

    SDL_Window *win = SDL_CreateWindow(
        "C Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_SIZE_X, WINDOW_SIZE_Y,
        SDL_WINDOW_SHOWN
    );
    
    if (!win) {
        SDL_Log("SDL_CreateWindow error: %s", SDL_GetError());
        SDL_Quit();
        return;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(
        win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!ren) {
        SDL_Log("SDL_CreateRenderer error: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return;
    }

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    engine.window = win;
    engine.renderer = ren;
    engine.window_running = 1;
    
}