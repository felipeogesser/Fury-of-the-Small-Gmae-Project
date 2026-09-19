#ifndef CEM
#define CEM

#include "game_state.h"
#include "maps.h"
#include "unit.h"
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>

void calculate_unit_random_mov(Unit *get_ent, Map *map, GameState *game, Uint32 now1, Uint32 now2);

#endif