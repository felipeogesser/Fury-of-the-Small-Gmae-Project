#ifndef POC
#define POC

#include "objects.h"
#include "player.h"
#include "gameState.h"

void isPlayerCollidingObject(Player *player, float (*hitBoxObject)[4][2], GameState *game, int x);

#endif