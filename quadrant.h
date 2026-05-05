#ifndef QUADRANT
#define QUADRANT

#include <stdlib.h>
#include "maps.h"
#include "gameState.h"

typedef struct bigQuadrant {
    unsigned short id;
    unsigned short entity_count;
    unsigned char smell;
    unsigned short entitiesInsideQuadrantById[entities_created_count];
} bigQuadrant;

typedef struct mediumQuadrant {
    unsigned char smell;

} mediumQuadrant;

typedef struct smallQuadrant {
    unsigned char smell;

} smallQuadrant;

/*typedef struct EntitiesInsideQuadrant {
    int entitiesInsideQuadrantById[10];
    int entitiesInQuadrantCount;
} EntitiesInsideQuadrant;

typedef struct Quadrant {
    int id;
    struct EntitiesInsideQuadrant innerEntsInQuad;
    int objectsInQuadrant[MAX_OBJECTS];
} Quadrant;*/

//extern Quadrant *quadrant;

void calculateQuadrantSize(Map *map, GameState *game);

void calculateAmountOfQuadrants(Map *map, GameState *game);

void init_quadrants(GameState *game);

Quadrant *get_quadrant(int id);

#endif