#ifndef BATTLEPLAN_H
#define BATTLEPLAN_H

#include <SDL2/SDL.h>

void battleplan_init(void);

void battleplan_input(SDL_Event *e);

void battleplan_update(void);

void battleplan_render(void);

void battleplan_destroy(void);

void battleplan_render_grid(SDL_Renderer *renderer);

#endif