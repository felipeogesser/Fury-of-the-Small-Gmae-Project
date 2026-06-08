#ifndef GENERAL_H
#define GENERAL_H

#include "armies_types.h"
#include "game_state_types.h"
#include "general_types.h"

void init_general_battalion_taxonomy(void);

void create_generals(General *general, float ongoing_point_X, float ongoing_point_Y, unsigned int i);

void update_generals(Armies *armies, GameState *game);


#endif