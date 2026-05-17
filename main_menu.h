#ifndef MAIN_MENU_SCREEN_H
#define MAIN_MENU_SCREEN_H

#include <SDL2/SDL.h>

struct MainMenu {

    char background_R_color;
    char background_G_color;
    char background_B_color;
    char background_Alpha;

    short start_button_position_X;
    short start_button_position_Y;
    short start_button_width_X;
    short start_button_width_Y;
    short start_button_R_color;
    short start_button_G_color;
    short start_button_B_color;
    short start_button_Alpha;
    char *start_button_text;

};

extern struct MainMenu main_menu;

void main_menu_init(void);

void main_menu_input(SDL_Event *e);

void main_menu_render(void);

void main_menu_destroy(void);

#endif