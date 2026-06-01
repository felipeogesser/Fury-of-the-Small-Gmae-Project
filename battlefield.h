#ifndef BATTLEFIELD_H
#define BATTLEFIELD_H

#include <SDL2/SDL.h>

void battlefield_init(void);

void battlefield_input(SDL_Event *e);

void battlefield_update(void);

void battlefield_render(void);

void battlefield_destroy(void);

#endif