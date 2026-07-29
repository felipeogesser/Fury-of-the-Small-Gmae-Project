#include "main_menu.h"
#include "main_menu_internal.h"
#include "engine_internal.h"
#include "scene_handler.h"
#include "scenes.h"
#include "window_settings.h"
#include <SDL2/SDL_ttf.h>
#include <stddef.h>
#include <string.h>

static MainMenu main_menu = {0};

static TTF_Font *main_menu_font = NULL;

static SDL_Color main_menu_white = {255, 255, 255, 255};

static SDL_Surface *main_menu_surface = NULL; 

static SDL_Texture *message = NULL;

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

    main_menu.button_battlefield_position_x = 400;
    main_menu.button_battlefield_position_y = 400;
    main_menu.button_battlefield_width_x = 100;
    main_menu.button_battlefield_width_y = 40;
    main_menu.button_battlefield_R_color = 255;
    main_menu.button_battlefield_G_color = 255;
    main_menu.button_battlefield_B_color = 255;
    main_menu.button_battlefield_Alpha = 255;
    main_menu.button_battlefield_text = "battlefield";

    main_menu.button_battleplan_position_x = 800;
    main_menu.button_battleplan_position_y = 800;
    main_menu.button_battleplan_width_x = 100;
    main_menu.button_battleplan_width_y = 40;
    main_menu.button_battleplan_R_color = 255;
    main_menu.button_battleplan_G_color = 255;
    main_menu.button_battleplan_B_color = 255;
    main_menu.button_battleplan_Alpha = 255;
    main_menu.button_battlefield_text = "battleplan";

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

    message = SDL_CreateTextureFromSurface(renderer, main_menu_surface);
    SDL_FreeSurface(main_menu_surface);
    main_menu_surface = NULL;
    TTF_CloseFont(main_menu_font);
    main_menu_font = NULL;

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

        _Bool button_battlefield_clicked =
            mouse_x >= main_menu.button_battlefield_position_x &&
            mouse_x <  main_menu.button_battlefield_position_x +
            main_menu.button_battlefield_width_x &&
            mouse_y >= main_menu.button_battlefield_position_y &&
            mouse_y <  main_menu.button_battlefield_position_y +
            main_menu.button_battlefield_width_y;

        _Bool button_battleplan_clicked =
            mouse_x >= main_menu.button_battleplan_position_x &&
            mouse_x <  main_menu.button_battleplan_position_x +
            main_menu.button_battleplan_width_x &&
            mouse_y >= main_menu.button_battleplan_position_y &&
            mouse_y <  main_menu.button_battleplan_position_y +
            main_menu.button_battleplan_width_y;

        if (button_battlefield_clicked) {
            
            scene_switch(BATTLEFIELD);
    
        }

        if (button_battleplan_clicked) {

            scene_switch(BATTLEPLAN);
        
        }

    }

}

void main_menu_render(void) {
    
    if (message == NULL) return;

    SDL_Renderer *renderer = engine.renderer;

    //SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    //SDL_RenderClear(renderer);

    SDL_Rect background = {0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y};
    SDL_SetRenderDrawColor(
        renderer,
        main_menu.background_R_color,
        main_menu.background_G_color,
        main_menu.background_B_color,
        main_menu.background_Alpha
    );
    SDL_RenderFillRect(renderer, &background);

    SDL_Rect button_battlefield = {
        main_menu.button_battlefield_position_x,
        main_menu.button_battlefield_position_y,
        main_menu.button_battlefield_width_x,
        main_menu.button_battlefield_width_y 
    };
    SDL_SetRenderDrawColor(
        renderer,
        main_menu.button_battlefield_R_color,
        main_menu.button_battlefield_G_color,
        main_menu.button_battlefield_B_color,
        main_menu.button_battlefield_Alpha
    );
    SDL_RenderFillRect(renderer, &button_battlefield);

    SDL_RenderCopy(renderer, message, NULL, &button_battlefield);

        SDL_Rect button_battleplan = {
        main_menu.button_battleplan_position_x,
        main_menu.button_battleplan_position_y,
        main_menu.button_battleplan_width_x,
        main_menu.button_battleplan_width_y 
    };
    SDL_SetRenderDrawColor(
        renderer,
        main_menu.button_battleplan_R_color,
        main_menu.button_battleplan_G_color,
        main_menu.button_battleplan_B_color,
        main_menu.button_battleplan_Alpha
    );
    SDL_RenderFillRect(renderer, &button_battleplan);

    SDL_RenderCopy(renderer, message, NULL, &button_battleplan);

    //SDL_RenderPresent(renderer);

}


void main_menu_destroy(void) {

    if (message) {
        SDL_DestroyTexture(message);
        message = NULL;
    }

    TTF_Quit();

}