#ifndef GENERAL_H
#define GENERAL_H

#include "general_types.h"
#include "armies_types.h"
#include "battleplan_types.h"
#include "game_state_types.h"

void init_general_battalion_taxonomy(void);

void init_general(General *general, General *general_from_battleplan_payload, Grid *grid, unsigned int x, unsigned int y);

void update_generals(Armies *armies, GameState *game);


#endif