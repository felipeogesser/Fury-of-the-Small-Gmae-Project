#include "quadrant.h"
#include "gameState.h"

int a, b, temp, amountX, amountY;

Quadrant *quadrant = NULL;

void calculateQuadrantSize(Map *map, GameState *game) {
    a = (int)map->mapSizeX;
    b = (int)map->mapSizeY;
    while (b != 0) {
        temp = b;
        b = a % b;
        a = temp;
    }
    game->quadrantSize = (int)a / 2;
}

void calculateAmountOfQuadrants(Map *map, GameState *game) {
    amountX = map->mapSizeX / game->quadrantSize;
    amountY = map->mapSizeY / game->quadrantSize;
}


void init_quadrants(int amountX, int amountY) {
    quadrant = calloc(amountX * amountY, sizeof *quadrant);
    int i;
    for (i = 0; i < amountX * amountY; i++) {
        quadrant[i].id = i + 1;
    }
}

Quadrant *get_quadrant(int id) {
    if (id <= 0) return NULL;
    return &quadrant[id];
}
