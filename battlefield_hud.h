#ifndef BATTLEFIELD_HUD_H
#define BATTLEFIELD_HUD_H

#include "table_internal.h"
#include <SDL2/SDL.h>

void battlefield_hud_reset(unsigned int battalion_count);

_Bool battlefield_hud_input(const SDL_Event *e);

void battlefield_hud_render(const Table *player_tables, const Table *enemy_tables);

_Bool battlefield_hud_selected_battalion(_Bool *is_enemy, unsigned int *index);

#endif
