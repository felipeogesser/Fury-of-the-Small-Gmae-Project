#include "main_menu.h"
#include "engine.h"
#include "scene_handler.h"
#include "scenes.h"
#include "windowSettings.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

struct MainMenu main_menu = {0};

TTF_Font *font = NULL;

SDL_Color white = {255, 255, 255, 255};

SDL_Surface *surface = NULL; 

SDL_Texture *message = NULL;

void main_menu_init(void) {

    SDL_Renderer *renderer = engine.renderer;

    engine.main_menu = &main_menu;

    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init failed: %s", TTF_GetError());
    }
    
    main_menu.background_R_color = 200;
    main_menu.background_G_color = 54;
    main_menu.background_B_color = 142;
    main_menu.background_Alpha = 255;

    main_menu.start_button_position_X = 400;
    main_menu.start_button_position_Y = 400;
    main_menu.start_button_width_X = 100;
    main_menu.start_button_width_Y = 40;
    main_menu.start_button_R_color = 255;
    main_menu.start_button_G_color = 255;
    main_menu.start_button_B_color = 255;
    main_menu.start_button_Alpha = 255;
    main_menu.start_button_text = "start game";

    font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!font) {
        SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
        return;
    }

    surface = TTF_RenderText_Solid(font, "play button", white);
    if (!surface) {
        SDL_Log("TTF_RenderText_Solid failed: %s", TTF_GetError());
        return;
    }

    message = SDL_CreateTextureFromSurface(renderer, surface);
    SDL_FreeSurface(surface);
    surface = NULL;
    TTF_CloseFont(font);
    font = NULL;

    if (!message) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return;
    }

}

void main_menu_input(SDL_Event *e) {

    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_LEFT) {

        int mouse_x = e->button.x;
        int mouse_y = e->button.y;

        _Bool button_clicked =
            mouse_x >= main_menu.start_button_position_X &&
            mouse_x <  main_menu.start_button_position_X +
            main_menu.start_button_width_X &&
            mouse_y >= main_menu.start_button_position_Y &&
            mouse_y <  main_menu.start_button_position_Y +
            main_menu.start_button_width_Y;

        if (button_clicked) {
            
            scene_switch(BATTLEFIELD);
    
        }

    }

}

void main_menu_render(void) {
    
    if (message == NULL) return;

    SDL_Renderer *renderer = engine.renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect main_menu_background = {0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y};
    SDL_SetRenderDrawColor(
        renderer,
        main_menu.background_R_color,
        main_menu.background_G_color,
        main_menu.background_B_color,
        main_menu.background_Alpha
    );
    SDL_RenderFillRect(renderer, &main_menu_background);

    SDL_Rect main_menu_start_button = {
        main_menu.start_button_position_X,
        main_menu.start_button_position_Y,
        main_menu.start_button_width_X,
        main_menu.start_button_width_Y 
    };
    SDL_SetRenderDrawColor(
        renderer,
        main_menu.start_button_R_color,
        main_menu.start_button_G_color,
        main_menu.start_button_B_color,
        main_menu.start_button_Alpha
    );
    SDL_RenderFillRect(renderer, &main_menu_start_button);

    SDL_RenderCopy(renderer, message, NULL, &main_menu_start_button);

    SDL_RenderPresent(renderer);

}


void main_menu_destroy(void) {

    if (message) {
        SDL_DestroyTexture(message);
        message = NULL;
    }

    TTF_Quit();

}