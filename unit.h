#ifndef UNIT_H
#define UNIT_H

#include "armies_types.h"
#include "unit_types.h"
#include "game_state_types.h"
#include "general_types.h"
    
Unit *get_unit(int id);

void init_unit(void);

void create_unit(Unit *unit, General *general, float ongoing_point_X, float ongoing_point_Y, unsigned int i, unsigned int k);

void update_units(Armies *armies, GameState *game);

#endif