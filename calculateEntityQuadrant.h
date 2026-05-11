#ifndef CEQ
#define CEQ

#include <SDL2/SDL.h>
#include "armies.h"
#include "entities.h"
#include "gameState.h"
#include "grids.h"

#define QUAD1 (1 << 0)
#define QUAD2 (1 << 1)
#define QUAD3 (1 << 2)
#define QUAD4 (1 << 3)

#define OOBQUAD2 (1 << 1)
#define OOBQUAD3 (1 << 2)
#define OOBQUAD4 (1 << 3)

void initialCheckEntityQuadrant(Armies *armies, GameState *game, Grids *grids);
void checkEntityQuadrant(Armies *armies, GameState *game, Grids *grids);
void renderQuadrantsSetup(Armies *armies, GameState *game);
void renderQuadrants(Entity *entities, GameState *game, SDL_Renderer *renderer);

#endif