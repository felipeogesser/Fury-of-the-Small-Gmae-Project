#ifndef SCENE_REGISTRY_H
#define SCENE_REGISTRY_H

#include "scenes.h"
#include <SDL2/SDL.h>

typedef void (*function_pointer)(void);

struct SceneCallbacks {
    function_pointer init;
    void (*input)(SDL_Event *e);
    function_pointer update;
    function_pointer render;
    function_pointer destroy;
};

extern const struct SceneCallbacks scene_registry[SCENE_COUNT];

#endif