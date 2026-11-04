#ifndef SETTINGS_MENU_H
#define SETTINGS_MENU_H

#include <SDL2/SDL.h>

void settings_menu_init(void);

void settings_menu_input(SDL_Event *e);

void settings_menu_render(void);

void settings_menu_destroy(void);

#endif
