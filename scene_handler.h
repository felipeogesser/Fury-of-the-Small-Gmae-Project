#ifndef SCENE_HANDLER_H
#define SCENE_HANDLER_H

#include "scenes.h"
#include <SDL2/SDL.h>

//void scene_handler_init(void);

void scene_switch(enum SceneState next_scene);

void scene_input(SDL_Event *e);

void scene_render(void);

#endif