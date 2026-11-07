#ifndef STAGE_MAP_H
#define STAGE_MAP_H

#include <SDL2/SDL.h>

void stage_map_init(void);

void stage_map_input(SDL_Event *e);

void stage_map_update(float dt);

void stage_map_render(void);

void stage_map_destroy(void);

#endif
