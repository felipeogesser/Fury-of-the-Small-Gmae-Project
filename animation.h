#ifndef ANIMATION_H
#define ANIMATION_H

#include <SDL2/SDL.h>

void animation_init(void);
void animation_input(SDL_Event *e);
void animation_update(void);
void animation_render(void);
void animation_destroy(void);

#endif