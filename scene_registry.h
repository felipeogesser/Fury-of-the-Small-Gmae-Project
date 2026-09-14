#ifndef SCENE_REGISTRY_H
#define SCENE_REGISTRY_H

#include "scenes.h"
#include <SDL2/SDL.h>

typedef void (*function_pointer)(void);

typedef struct SceneCallbacks {
    void (*init)(void);
    void (*input)(SDL_Event *e);
    void (*update)(void);
    void (*render)(void);
    void (*destroy)(void);
} SceneCallbacks;

extern const SceneCallbacks scene_registry[SCENE_COUNT];

#endif