#include "battleplan.h"
#include "battleplan_internal.h"
#include "engine_internal.h"
#include "scene_handler.h"
#include "scenes.h"
#include "window_settings.h"
#include <SDL2/SDL_ttf.h>
#include <stddef.h>
#include <string.h>

Battleplan battleplan = {0};

TTF_Font *battleplan_font = NULL;

SDL_Color battleplan_white = {255, 255, 255, 255};

SDL_Surface *battleplan_surface = NULL; 

SDL_Texture *battleplan_message = NULL;

void battleplan_init(void) {

    SDL_Renderer *renderer = engine.renderer;

    engine.battleplan = &battleplan;

    if (TTF_Init() == -1) {
        SDL_Log("TTF_Init failed: %s", TTF_GetError());
    }
    
    battleplan.background_R_color = 130;
    battleplan.background_G_color = 43;
    battleplan.background_B_color = 95;
    battleplan.background_Alpha = 255;

    battleplan.battleplan_button_position_X = 400;
    battleplan.battleplan_button_position_Y = 400;
    battleplan.battleplan_button_width_X = 100;
    battleplan.battleplan_button_width_Y = 40;
    battleplan.battleplan_button_R_color = 255;
    battleplan.battleplan_button_G_color = 255;
    battleplan.battleplan_button_B_color = 255;
    battleplan.battleplan_button_Alpha = 255;
    battleplan.battleplan_button_text = "Go back to main menu";

    battleplan_font = TTF_OpenFont("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf", 24);
    if (!battleplan_font) {
        SDL_Log("TTF_OpenFont failed: %s", TTF_GetError());
        return;
    }

    battleplan_surface = TTF_RenderText_Solid(battleplan_font, "play button", battleplan_white);
    if (!battleplan_surface) {
        SDL_Log("TTF_RenderText_Solid failed: %s", TTF_GetError());
        return;
    }

    battleplan_message = SDL_CreateTextureFromSurface(renderer, battleplan_surface);
    SDL_FreeSurface(battleplan_surface);
    battleplan_surface = NULL;
    TTF_CloseFont(battleplan_font);
    battleplan_font = NULL;

    if (!battleplan_message) {
        SDL_Log("SDL_CreateTextureFromSurface failed: %s", SDL_GetError());
        return;
    }

}

void battleplan_input(SDL_Event *e) {

    if (e->type == SDL_MOUSEBUTTONDOWN &&
        e->button.button == SDL_BUTTON_LEFT) {

        int mouse_x = e->button.x;
        int mouse_y = e->button.y;

        _Bool button_clicked =
            mouse_x >= battleplan.battleplan_button_position_X &&
            mouse_x <  battleplan.battleplan_button_position_X +
            battleplan.battleplan_button_width_X &&
            mouse_y >= battleplan.battleplan_button_position_Y &&
            mouse_y <  battleplan.battleplan_button_position_Y +
            battleplan.battleplan_button_width_Y;

        if (button_clicked) {
            
            scene_switch(BATTLEFIELD);
    
        }

    }

}

void battleplan_update(void) {

}

void battleplan_render(void) {

    if (battleplan_message == NULL) return;

    SDL_Renderer *renderer = engine.renderer;

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_Rect battleplan_background = {0, 0, WINDOW_SIZE_X, WINDOW_SIZE_Y};
    SDL_SetRenderDrawColor(
        renderer,
        battleplan.background_R_color,
        battleplan.background_G_color,
        battleplan.background_B_color,
        battleplan.background_Alpha
    );
    SDL_RenderFillRect(renderer, &battleplan_background);

    SDL_Rect battleplan_button = {
        battleplan.battleplan_button_position_X,
        battleplan.battleplan_button_position_Y,
        battleplan.battleplan_button_width_X,
        battleplan.battleplan_button_width_Y 
    };
    SDL_SetRenderDrawColor(
        renderer,
        battleplan.battleplan_button_R_color,
        battleplan.battleplan_button_G_color,
        battleplan.battleplan_button_B_color,
        battleplan.battleplan_button_Alpha
    );
    SDL_RenderFillRect(renderer, &battleplan_button);

    SDL_RenderCopy(renderer, battleplan_message, NULL, &battleplan_button);

    battleplan_render_grid(renderer);

    SDL_RenderPresent(renderer);

}

void battleplan_destroy(void) {
    
    if (battleplan_message) {
        SDL_DestroyTexture(battleplan_message);
        battleplan_message = NULL;
    }

    TTF_Quit();
}



void battleplan_render_grid(SDL_Renderer *renderer) {

    signed int grid_dimension_x = 11;
    signed int grid_dimension_y = 11;

    signed int offset = 100;
    SDL_Rect battleplan_grid = {
        offset,
        offset,
        (signed int)WINDOW_SIZE_X - offset,
        (signed int)WINDOW_SIZE_Y - offset
    };
    SDL_SetRenderDrawColor(
        renderer,
        128,
        128,
        128,
        255
    );
    SDL_RenderFillRect(renderer, &battleplan_grid);

    signed int cell_position_x = 120;
    signed int cell_position_y = 120;
    signed int cell_width_x = 30;
    signed int cell_width_y = 30;
    for (unsigned int i = 0; i < 2; i++) {

        for (signed int j = 0; j < grid_dimension_x; j++) {

            for (signed int k = 0; k < grid_dimension_y; k++) {



                SDL_Rect battleplan_grid_cell = {
                    cell_position_x,
                    cell_position_y,
                    cell_width_x,
                    cell_width_y
                };
                SDL_SetRenderDrawColor(
                    renderer,
                    200,
                    200,
                    200,
                    255
                );
                SDL_RenderFillRect(renderer, &battleplan_grid_cell);

                cell_position_y += cell_width_y + 10;

            }

            cell_position_x += cell_width_x + 10;
            cell_position_y = 120;
        }

        cell_position_x += 100;
        cell_position_y = 120;

    }

}