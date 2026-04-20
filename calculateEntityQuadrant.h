#ifndef CEQ
#define CEQ

#include <SDL2/SDL.h>
#include "entities.h"
#include "quadrant.h"
#include "gameState.h"

#define QUAD1 (1 << 0)
#define QUAD2 (1 << 1)
#define QUAD3 (1 << 2)
#define QUAD4 (1 << 3)

#define OOBQUAD2 (1 << 1)
#define OOBQUAD3 (1 << 2)
#define OOBQUAD4 (1 << 3)

void initialCheckEntityQuadrant(GameState *game, Entity *entities, Quadrant *quadrant, int y);
void checkEntityQuadrant(GameState *game, Entity *entities, Quadrant *quadrant, int y);
void renderQuadrantsSetup(SDL_Renderer *ren, GameState *game, Entity *entities, Quadrant *quadrant, int y);
void renderQuadrants(SDL_Renderer *ren, GameState *game, Entity *entities, Quadrant *quadrant, int y);

#endif