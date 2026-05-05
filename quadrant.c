#include "quadrant.h"
#include "gameState.h"

Quadrant *quadrant = NULL;

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

void calculateAmountOfQuadrants(Map *map, GameState *game) {
    amountX = map->mapSizeX / game->low_LOD_quadrant_size;
    amountY = map->mapSizeY / game->low_LOD_quadrant_size;
    game->low_LOD_quadrant_count = amountX * amountY;
    game->amountX = amountX;
    game->amountY = amountY;
}

void fill_quadrant_data(GameState *game, Grids *grids) {
    bigQuadrant *bigQuad = grids->GLLoD->bigQuad;
    unsigned int amountX = game->amountX;
    unsigned int amountY = game->amountY;
    for (unsigned int i = 0; i < amountX * amountY; i++) {
        bigQuad[i].id = i + 1;
    }
}

Quadrant *get_quadrant(int id) {
    if (id <= 0) return NULL;
    return &quadrant[id - 1];
}
