#include "quadrant.h"
#include "gameState.h"

int a, b, temp;

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
    amountX = map->mapSizeX / game->quadrantSize;
    amountY = map->mapSizeY / game->quadrantSize;
    game->high_LOD_quadrant_count = amountX * amountY;
    game->amountX;
    game->amountY;

}


void init_quadrants(GameState *game) {
    unsigned int amountX = game->amountX;
    unsigned int amountY = game->amountY;
    quadrant = calloc(amountX * amountY, sizeof *quadrant);
    for (unsigned int i = 0; i < amountX * amountY; i++) {
        quadrant[i].id = i + 1;
    }

}

Quadrant *get_quadrant(int id) {
    if (id <= 0) return NULL;
    return &quadrant[id - 1];
}
