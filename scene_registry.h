#ifndef SCENE_REGISTRY_H
#define SCENE_REGISTRY_H

#include "scenes.h"
#include <SDL2/SDL.h>

typedef void (*function_pointer)(void);

struct SceneCallbacks {
    void (*init)(void);
    void (*input)(SDL_Event *e);
    void (*update)(void);
    void (*render)(void);
    void (*destroy)(void);
};

struct SceneCallbacks;

extern const struct SceneCallbacks scene_registry[SCENE_COUNT];

#endif