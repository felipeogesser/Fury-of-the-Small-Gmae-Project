#ifndef GAME_STATE_H
#define GAME_STATE_H

#include "game_state_types.h"
#include "player_types.h"

GameState *pass_pointer(void);

void update_game_state(Player *player);

#endif