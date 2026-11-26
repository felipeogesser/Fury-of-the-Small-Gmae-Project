#ifndef BATTLEFIELD_HUD_H
#define BATTLEFIELD_HUD_H

#include "table_internal.h"
#include <SDL2/SDL.h>

enum BattlefieldHudArea {
    BATTLEFIELD_HUD_POWER_BAR,
    BATTLEFIELD_HUD_TABS,
    BATTLEFIELD_HUD_CARDS,
    BATTLEFIELD_HUD_DETAIL
};

void battlefield_hud_reset(unsigned int battalion_count, unsigned int battalion_size);
_Bool battlefield_hud_input(const SDL_Event *e);
void battlefield_hud_render(const Table *player_tables, const Table *enemy_tables);
_Bool battlefield_hud_selected_battalion(_Bool *is_enemy, unsigned int *index);
void battlefield_hud_select(_Bool is_enemy, unsigned int index);
_Bool battlefield_hud_is_showing_enemy(void);
SDL_Rect battlefield_hud_area(enum BattlefieldHudArea area);

#endif
