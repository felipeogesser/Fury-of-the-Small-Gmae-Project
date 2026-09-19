#include "quadrant.h"
#include "quadrant_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "battlefield_grid_internal.h"
#include "maps_internal.h"

void calculateAmountOfQuadrants(void) {
    engine.game->low_LOD_quadrant_size = 64U;
    unsigned short amountX = (unsigned short)(engine.map->mapSizeX / engine.game->low_LOD_quadrant_size);
    unsigned short amountY = (unsigned short)(engine.map->mapSizeY / engine.game->low_LOD_quadrant_size);
    engine.game->amountX = amountX;
    engine.game->amountY = amountY;
    engine.game->low_LOD_quadrant_count = (unsigned short)(amountX * amountY);
    engine.game->medium_LOD_quadrant_count = (unsigned short)(amountX * amountY * 4);
    engine.game->high_LOD_quadrant_count = (unsigned short)(amountX * amountY * 16);
}

void fill_quadrant_data(void) {
    bigQuadrant *bigQuad = engine.battlefield_grid->GLLoD->bigQuad;
    unsigned short amountX = engine.game->amountX;
    unsigned short amountY = engine.game->amountY;
    for (unsigned short i = 0; i < amountX * amountY; i++) {
        
        bigQuad[i].id = i + 1;

    }

}
