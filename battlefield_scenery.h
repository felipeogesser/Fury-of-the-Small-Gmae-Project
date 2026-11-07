#ifndef BATTLEFIELD_SCENERY_H
#define BATTLEFIELD_SCENERY_H

#include "maps_types.h"
#include <SDL2/SDL.h>

void battlefield_scenery_render_background(SDL_Renderer *renderer, const Map *map);

void battlefield_scenery_render_foreground(SDL_Renderer *renderer);

void battlefield_scenery_destroy(void);

#endif
