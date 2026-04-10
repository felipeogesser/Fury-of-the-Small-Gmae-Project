#ifndef POC
#define POC

#include "objects.h"
#include "player.h"
#include "gameState.h"

void isPlayerCollidingObject(float hitBoxPlayer[4][2], float (*hitBoxObject)[4][2], GameState *game, int x);

#endif