#ifndef CEM
#define CEM

#include "gameState.h"
#include "maps.h"
#include "entities.h"
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

void calculateEntityRandomMov(Entity *get_ent, Map *map, GameState *game, Uint32 now1, Uint32 now2);

#endif