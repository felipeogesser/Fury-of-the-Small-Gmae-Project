#ifndef PLAYER_COLLISION_H
#define PLAYER_COLLISION_H

#include "game_state_types.h"
#include "maps_types.h"
//#include "objects.h"
#include "player_types.h"

void player_map_edge_collision(GameState *game, Map *map, Player *player);

//void isPlayerCollidingObject(Player *player, float (*hitBoxObject)[4][2], GameState *game, int x);

#endif