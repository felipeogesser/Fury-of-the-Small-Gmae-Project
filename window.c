#include "window.h"
#include "engine_internal.h"
#include "window_settings.h"
#include <SDL2/SDL.h>

static void create_SDL_window(void) {
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        fprintf(stderr, "renderer creation failed\n");
        exit(EXIT_FAILURE);
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
        fprintf(stderr, "window creation failed\n");
        exit(EXIT_FAILURE);
    }

    SDL_Renderer *ren = SDL_CreateRenderer(
        win, -1,
        //SDL_RENDERER_SOFTWARE
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    if (!ren) {
        SDL_Log("SDL_CreateRenderer error: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        fprintf(stderr, "renderer creation failed\n");
        exit(EXIT_FAILURE);
    }

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    engine.window = win;
    engine.renderer = ren;
    engine.window_running = 1;
    
}

void create_window(void) {

    create_SDL_window();

}

static void destroy_SDL_window(void) {

    SDL_DestroyRenderer(engine.renderer);
    SDL_DestroyWindow(engine.window);
    SDL_Quit();

}

void destroy_window(void) {

    destroy_SDL_window();

}