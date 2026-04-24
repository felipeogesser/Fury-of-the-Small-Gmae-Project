#ifndef EEE
#define EEE

#include <string.h>
#include "entities.h"
#include "quadrant.h"
#include "gameState.h"
#include "maps.h"
#include "player.h"

void entityFindEnemy(Entity *entities, Quadrant *quadrant, int y, int q);
void entityEngageEnemy(Entity *entities, GameState *game, Map *map, int y);

#endif