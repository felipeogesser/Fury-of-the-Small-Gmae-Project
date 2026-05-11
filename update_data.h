#ifndef UPDATE_ENTITIES_H
#define UPDATE_ENTITIES_H

#include "gameState.h"
#include "entities.h"
#include "player.h"

void update_player(GameState *game, Player *player);
void update_units(Entity *entities, GameState *game);
void update_game_state(GameState *game);
void update_game_data(Entity *entities, GameState *game, Player *player);

#endif