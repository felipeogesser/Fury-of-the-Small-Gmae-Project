#ifndef UNIT_H
#define UNIT_H

#include "unit_types.h"
#include "armies_types.h"
#include "battalion_types.h"
#include "battleplan_types.h"
#include "game_state_types.h"
#include "general_types.h"

Unit *get_unit(int id);

void init_units(Battalion *battalion, Grid *grid, OccupiedCell *occupied_cell);

void update_units(Armies *armies, GameState *game);

void set_units_position(Unit *unit, unsigned int battalion_size, Battalion *battalion, Grid *grid, OccupiedCell *occupied_cell);


#endif