#ifndef MAIN_MENU_H
#define MAIN_MENU_H

#include <SDL2/SDL.h>

void main_menu_init(void);

void main_menu_input(SDL_Event *e);

void main_menu_render(void);

void main_menu_destroy(void);

#endif