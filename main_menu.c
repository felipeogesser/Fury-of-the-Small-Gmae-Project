#include "main_menu.h"
#include "windowSettings.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

struct MainMenu main_menu = {0};

TTF_Font* font = NULL;

SDL_Color white = {0};

SDL_Surface* surface = NULL; 

SDL_Texture* message = NULL;

void main_menu_init(void) {

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

    font = TTF_OpenFont("Sans.ttf", 24);

    white = {255, 255, 255, 255};

    surface = TTF_RenderText_Solid(font, "put your text here", white); 

    message = SDL_CreateTextureFromSurface(renderer, surface);

    SDL_FreeSurface(surface);

}

void render_main_menu_screen(struct MainMenu main_menu, struct SDL_Renderer *renderer) {
    
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

    SDL_Rect message_rect;
    message_rect.x = 0;
    message_rect.y = 0;
    message_rect.w = 100;
    message_rect.h = 100;

    SDL_RenderCopy(renderer, message, NULL, &message_rect);

    //SDL_FreeSurface(surfaceMessage);
    //SDL_DestroyTexture(Message);

}