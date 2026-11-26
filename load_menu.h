#ifndef LOAD_MENU_H
#define LOAD_MENU_H

#include <SDL2/SDL.h>

void load_menu_init(void);
void load_menu_input(SDL_Event *e);
void load_menu_render(void);
void load_menu_destroy(void);

#endif
