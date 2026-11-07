#ifndef BATTLEPLAN_SCENERY_H
#define BATTLEPLAN_SCENERY_H

#include <SDL2/SDL.h>

void battleplan_scenery_init(SDL_Renderer *renderer);

_Bool battleplan_scenery_render(SDL_Renderer *renderer);

void battleplan_scenery_destroy(void);

#endif
