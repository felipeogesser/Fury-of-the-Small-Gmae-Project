#ifndef TUTORIAL_H
#define TUTORIAL_H

#include "scenes.h"
#include "shared_memory_control.h"
#include <SDL2/SDL.h>

_Bool tutorial_is_active(void);
void tutorial_scene_entered(enum Scene scene);
_Bool tutorial_input(enum Scene scene, const SDL_Event *e);
void tutorial_render(enum Scene scene);
_Bool tutorial_holds_battle(void);
_Bool tutorial_record_battle(enum EpisodeResult result);

#endif
