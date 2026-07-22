#ifndef ENTITY_COLLISION_H
#define ENTITY_COLLISION_H

#include <SDL2/SDL.h>
#include "armies_types.h"
#include "entities_types.h"
#include "game_state_types.h"
#include "grids_types.h"

void initialCheckEntityQuadrant(Armies *armies, GameState *game, Grids *grids);
void check_entity_quadrant(Armies *armies, GameState *game, Grids *grids);
void renderQuadrantsSetup(Armies *armies, GameState *game);
void renderQuadrants(Entity *entities, GameState *game, SDL_Renderer *renderer);

#endif