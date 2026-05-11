#include "smell.h"
#include "quadrant.h"

void check_for_occupied_quadrants(Armies *armies, GameState *game, Grids *grids) {
    
    unsigned int *index = grids->GLLoD->occupied->index;
    unsigned int *occupied_count = grids->GLLoD->occupied->occupied_count;
    bigQuadrant *bigQuad =grids->GLLoD->bigQuad;
    unsigned int low_LOD_quadrant_size = game->low_LOD_quadrant_size;
    signed int amountX = game->amountX;

    Entity *entities = armies->batallions->entities;

    for (int i = 0; i < y; i++) {  
        unsigned int column = entities[i].positionX / low_LOD_quadrant_size;
        unsigned int row = entities[i].positionY / low_LOD_quadrant_size;

        unsigned int indexer = row * amountX + column;

        if (bigQuad[indexer].entity_count == 0) {
            index[(*occupied_count)++] = indexer;
        }

        bigQuad[indexer].entity_count++;
    
    }
}


void smell_value(Armies *armies, GameState *game, Grids *grids) {

    signed int amountX = game->amountX;
    float decay_rate = game->decay_rate;
    unsigned int big_quadrant_count = game->big_quadrant_count / 16;
    bigQuadrant *bigQuad =grids->GLLoD->bigQuad;
    unsigned int number_of_armies = 2;

    Army *army = armies->army;

    for (unsigned int h = 0; h < number_of_armies; h++) {
        
        unsigned int army_size = army[h].army_size;
        float entities_alive = army[h].entities_alive;

        for (unsigned int i = 0; i < army_size; i++) {
            
            if (entities_alive <= 0) return;
            if (entities_alive <= 0) return;
            
            unsigned int *occupied_count = grids->GLLoD->occupied->occupied_count;

            for (unsigned int j = 0; j < occupied_count; j++) {

                signed int index = grids->GLLoD->occupied->index[j];
                float local_entities_count = grids->GLLoD->bigQuad[index].local_entities_count;
                
                signed int col1 = index % amountX;
                signed int row1 = index / amountX;

                for (signed int k = 0; k < big_quadrant_count; k++) {

                    signed int col2 = k % amountX;
                    signed int row2 = k / amountX;
                    
                    signed int drow = row2 - row1;
                    signed int dcol = col2 - col1;

                    signed int mask_row = drow >> 31;
                    signed int mask_col = dcol >> 31;

                    drow = (drow ^ mask_row) - mask_row;
                    dcol = (dcol ^ mask_col) - mask_col;

                    signed int distance = drow + dcol;

                    float mass_ratio =
                        local_entities_count / entities_alive;
                    
                    unsigned char smell =
                        255.0f * mass_ratio * expf(-decay_rate * distance);

                    //if (smell > 255.0f) return 255;
                    //if (smell < 0.0f) return 0;

                    *bigQuad[k].smell += smell;
                }
            }
        }
    }
}