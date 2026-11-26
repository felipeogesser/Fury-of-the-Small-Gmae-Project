#ifndef BATTLE_CONTROLS_H
#define BATTLE_CONTROLS_H

#include <SDL2/SDL.h>

void battle_controls_reset(void);
_Bool battle_controls_input(const SDL_Event *e);
_Bool battle_controls_is_paused(void);
float battle_controls_time_scale(void);
SDL_Rect battle_controls_rect(void);
void battle_controls_render(void);

#endif
