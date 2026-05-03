#include "smell.h"

void check_for_occupied_quadrants(Entity *entities, GameState *game, Grids *grids, Occupied *occupied) {
    
    unsigned int *index = grids->GLLoD->occupied->index;
    unsigned int *occupied_count = grids->GLLoD->occupied->occupied_count;
    bigQuadrant *bigQuad =grids->GLLoD->bigQuad;
    unsigned int quadrant_size = game->quadrantSize;
    signed int amountX = game->amountX;

    for (int i = 0; i < y; i++) {  
        unsigned int column = entities[i].positionX / quadrant_size * 16;
        unsigned int row = entities[i].positionY / quadrant_size * 16;

        unsigned int indexer = row * amountX + column;

        if (*bigQuad[indexer].entity_count == 0) {
            *index[(*occupied_count)++] = indexer;
        }

        *bigQuad[indexer].entity_count++;
    
    }
}


void smell_value(GameState *game, Grids *grids, Teams *teams) {

    signed int amountX = game->amountX;
    float decay_rate = game->decay_rate;
    unsigned int big_quadrant_count = game->big_quadrant_count / 16;
    bigQuadrant *bigQuad =grids->GLLoD->bigQuad;

    for (unsigned int i = 0; i < teams->teams_count; i++) {

        float total_alive_team_entities = teams->team[i].total_alive_team_entities;
        
        if (total_alive_team_entities <= 0) return;
        if (total_alive_team_entities <= 0) return;
        
        unsigned int *occupied_count = grids->GLLoD->occupied->occupied_count;

        for (unsigned int j = 0; j < occupied_count; j++) {

            signed int index = grids->GLLoD->occupied->index[j];
            float local_team_entities = grids->GLLoD->bigQuad[index].local_team_entities;
            
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
                    local_team_entities / total_alive_team_entities;
                
                unsigned char smell =
                    255.0f * mass_ratio * expf(-decay_rate * distance);

                //if (smell > 255.0f) return 255;
                //if (smell < 0.0f) return 0;

                *bigQuad[k].smell += smell;
            }
        }
    }
}
