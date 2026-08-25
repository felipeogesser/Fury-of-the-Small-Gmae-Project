#ifndef GENERAL_H
#define GENERAL_H

#include "general_types.h"
#include "armies_types.h"
#include "battalion_types.h"
#include "game_state_types.h"

void init_general_battalion_taxonomy(void);

void init_generals(Battalion *battalion, General *general);

void update_generals(Armies *armies, GameState *game);


#endif