#include "quadrant.h"
#include "quadrant_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "grids_internal.h"
#include "maps_internal.h"

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
    unsigned short amountX = (unsigned short)(engine.map->mapSizeX / engine.game->low_LOD_quadrant_size);
    unsigned short amountY = (unsigned short)(engine.map->mapSizeY / engine.game->low_LOD_quadrant_size);
    engine.game->amountX = amountX;
    engine.game->amountY = amountY;
    engine.game->low_LOD_quadrant_count = (unsigned short)(amountX * amountY);
    engine.game->medium_LOD_quadrant_count = (unsigned short)(amountX * amountY * 4);
    engine.game->high_LOD_quadrant_count = (unsigned short)(amountX * amountY * 16);
}

void fill_quadrant_data(void) {
    bigQuadrant *bigQuad = engine.grids->GLLoD->bigQuad;
    unsigned short amountX = engine.game->amountX;
    unsigned short amountY = engine.game->amountY;
    for (unsigned short i = 0; i < amountX * amountY; i++) {
        bigQuad[i].id = i + 1;
        //for (unsigned int j; j < 100; j++) {
            //bigQuad[i].entities_inside_quad[j] = (unsigned int)-1;
        //}
    }

    /*for (unsigned int i = 0; i < engine.game->low_LOD_quadrant_count; i++) {
        engine.grids->GLLoD->occupied_quads_indexes[i] = (unsigned int)-1;
    }*/
}

/*Quadrant *get_quadrant(int id) {
    if (id <= 0) return NULL;
    return &quadrant[id - 1];
}*/
