#ifndef UNIT_H
#define UNIT_H

#include "armies_types.h"
#include "battalion_types.h"
#include "battleplan_grid_types.h"
#include "game_state_types.h"
#include "general_types.h"

void init_units(Battalion *battalion, unsigned int x, unsigned int y, _Bool is_enemy_army);

void update_units(Armies *armies, GameState *game);

#endif