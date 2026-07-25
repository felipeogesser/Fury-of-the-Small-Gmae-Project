#include "main_menu.h"
#include "main_menu_internal.h"
#include "engine_internal.h"
#include "scene_handler.h"
#include "scenes.h"
#include "window_settings.h"
#include <SDL2/SDL_ttf.h>
#include <stddef.h>
#include <string.h>

MainMenu main_menu = {0};

TTF_Font *main_menu_font = NULL;

SDL_Color main_menu_white = {255, 255, 255, 255};

SDL_Surface *main_menu_surface = NULL; 

SDL_Texture *main_menu_message = NULL;

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

    main_menu_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!main_menu_font) {
        SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
        return;
    }

    main_menu_surface = TTF_RenderText_Solid(main_menu_font, "play button", main_menu_white);
    if (!main_menu_surface) {
        SDL_Log("TTF_RenderText_Solid failed: %s", TTF_GetError());
        return;
    }

    main_menu_message = SDL_CreateTextureFromSurface(renderer, main_menu_surface);
    SDL_FreeSurface(main_menu_surface);
    main_menu_surface = NULL;
    TTF_CloseFont(main_menu_font);
    main_menu_font = NULL;

    if (!main_menu_message) {
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
    
    if (main_menu_message == NULL) return;

    SDL_Renderer *renderer = engine.renderer;

    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_RenderClear(renderer);

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

    SDL_RenderCopy(renderer, main_menu_message, NULL, &main_menu_start_button);

    //SDL_RenderPresent(renderer);

}


void main_menu_destroy(void) {

    if (main_menu_message) {
        SDL_DestroyTexture(main_menu_message);
        main_menu_message = NULL;
    }

    TTF_Quit();

}