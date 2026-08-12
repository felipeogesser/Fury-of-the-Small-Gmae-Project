#ifndef UNIT_COLLISIOM_H
#define UNIT_COLLISIOM_H

#include <SDL2/SDL.h>
#include "armies_types.h"
#include "unit_types.h"
#include "game_state_types.h"
#include "grids_types.h"

void initialCheckUnitQuadrant(Armies *armies, GameState *game, Grids *grids);
void check_unit_quadrant(Armies *armies, GameState *game, Grids *grids);
void renderQuadrantsSetup(Armies *armies, GameState *game);
void renderQuadrants(Unit *unit, GameState *game, SDL_Renderer *renderer);

#endif