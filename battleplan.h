#ifndef BATTLEPLAN_H
#define BATTLEPLAN_H

#include "general_types.h"
#include <SDL2/SDL.h>

enum BattleplanArea {
    BATTLEPLAN_AREA_PLAYER_GRID,
    BATTLEPLAN_AREA_ENEMY_GRID,
    BATTLEPLAN_AREA_PLAYER_HEADER,
    BATTLEPLAN_AREA_ENEMY_HEADER,
    BATTLEPLAN_AREA_DRAWER_HANDLE,
    BATTLEPLAN_AREA_DRAWER,
    BATTLEPLAN_AREA_START_BUTTON,
    BATTLEPLAN_AREA_BACK_BUTTON
};

void battleplan_init(void);

void battleplan_input(SDL_Event *e);

void battleplan_update(float dt);

void battleplan_render(void);

void battleplan_destroy(void);

SDL_Rect battleplan_area(enum BattleplanArea area);

_Bool battleplan_is_drawer_open(void);

unsigned int battleplan_deployed_count(void);

unsigned int battleplan_required_count(void);

General *battleplan_general_at(int x, int y, _Bool *is_enemy);

void battleplan_lock_exit(_Bool is_locked);

#endif