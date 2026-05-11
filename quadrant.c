#include "quadrant.h"
#include "engine.h"
#include "gameState.h"

//Quadrant *quadrant = NULL;

/*void calculateQuadrantSize(Map *map, GameState *game) {
    unsigned int a = (int)map->mapSizeX;
    unsigned int b = (int)map->mapSizeY;
    while (b != 0) {
        unsigned int temp = b;
        b = a % b;
        a = temp;
    }
    game->quadrantSize = (int)a / 2;
}*/

void calculateAmountOfQuadrants(void) {
    unsigned int amountX = engine.map->mapSizeX / engine.game->low_LOD_quadrant_size;
    unsigned int amountY = engine.map->mapSizeY / engine.game->low_LOD_quadrant_size;
    engine.game->amountX = amountX;
    engine.game->amountY = amountY;
    engine.game->low_LOD_quadrant_count = amountX * amountY;
    engine.game->medium_LOD_quadrant_count = amountX * amountY * 4;
    engine.game->high_LOD_quadrant_count = amountX * amountY * 16;
}

void fill_quadrant_data(void) {
    bigQuadrant *bigQuad = engine.grids->GLLoD->bigQuad;
    unsigned int amountX = engine.game->amountX;
    unsigned int amountY = engine.game->amountY;
    for (unsigned int i = 0; i < amountX * amountY; i++) {
        bigQuad[i].id = i + 1;
    }
}

/*Quadrant *get_quadrant(int id) {
    if (id <= 0) return NULL;
    return &quadrant[id - 1];
}*/
