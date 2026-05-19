#ifndef CEQ
#define CEQ

#include <SDL2/SDL.h>
#include "armies.h"
#include "entities.h"
#include "gameState.h"
#include "grids.h"

#define QUAD1 ((unsigned char)(1u << 0))
#define QUAD2 ((unsigned char)(1u << 1))
#define QUAD3 ((unsigned char)(1u << 2))
#define QUAD4 ((unsigned char)(1u << 3))

#define OOBQUAD2 ((unsigned char)(1u << 1))
#define OOBQUAD3 ((unsigned char)(1u << 2))
#define OOBQUAD4 ((unsigned char)(1u << 3))

void initialCheckEntityQuadrant(Armies *armies, GameState *game, Grids *grids);
void check_entity_quadrant(Armies *armies, GameState *game, Grids *grids);
void renderQuadrantsSetup(Armies *armies, GameState *game);
void renderQuadrants(Entity *entities, GameState *game, SDL_Renderer *renderer);

#endif