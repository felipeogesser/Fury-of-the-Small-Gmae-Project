#ifndef UNIT_H
#define UNIT_H

#include "unit_types.h"
#include "armies_types.h"
#include "battalion_types.h"
#include "battleplan_grid_types.h"
#include "game_state_types.h"
#include "general_types.h"

Unit *get_unit(int id);

void init_units(Battalion *battalion, unsigned int x, unsigned int y);

void update_units(Armies *armies, GameState *game);

#endif