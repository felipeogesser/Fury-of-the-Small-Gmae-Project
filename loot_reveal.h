#ifndef LOOT_REVEAL_H
#define LOOT_REVEAL_H

#include <SDL2/SDL.h>

void loot_reveal_reset(void);
_Bool loot_reveal_is_open(void);
_Bool loot_reveal_input(const SDL_Event *e);
void loot_reveal_render(void);

#endif
