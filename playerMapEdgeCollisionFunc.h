#ifndef PMEC
#define PMEC

#include "player.h"
#include "maps.h"
#include "gameState.h"
#include <stdio.h>

void player_map_edge_collision(GameState *game, Map *map, Player *player);

#endif