#ifndef PERIPHERALS_H
#define PERIPHERALS_H

#include "gameState.h"
#include "player.h"

void process_peripherals(GameState *game, Player *player, _Bool *window_running);
//static void get_keyboard_input(GameState *game, Player *player, _Bool *window_running);

#endif