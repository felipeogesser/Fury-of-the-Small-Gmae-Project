#ifndef MAIN_MENU_SCREEN_H
#define MAIN_MENU_SCREEN_H

#include <SDL2/SDL.h>

struct MainMenu {

    unsigned char background_R_color;
    unsigned char background_G_color;
    unsigned char background_B_color;
    unsigned char background_Alpha;

    unsigned short start_button_position_X;
    unsigned short start_button_position_Y;
    unsigned short start_button_width_X;
    unsigned short start_button_width_Y;
    unsigned char start_button_R_color;
    unsigned char start_button_G_color;
    unsigned char start_button_B_color;
    unsigned char start_button_Alpha;
    const char *start_button_text;

};

extern struct MainMenu main_menu;

void main_menu_init(void);

void main_menu_input(SDL_Event *e);

void main_menu_render(void);

void main_menu_destroy(void);

#endif