#ifndef QUADRANT
#define QUADRANT

#include <stdlib.h>
#include "maps.h"
#include "gameState.h"

extern int a, b, q, temp, amountX, amountY;


typedef struct EntitiesInsideQuadrant {
    int entitiesInsideQuadrantById[10];
    int entitiesInQuadrantCount;
} EntitiesInsideQuadrant;


typedef struct Quadrant {
    int id;
    struct EntitiesInsideQuadrant innerEntsInQuad;
} Quadrant;

extern Quadrant *quadrant;

void calculateQuadrantSize(Map *map, GameState *game);

void calculateAmountOfQuadrants(Map *map, GameState *game);

void init_quadrants(int amountX, int amountY);

Quadrant *get_quadrant(int id);

#endif