#include "grids.h"
#include "grids_internal.h"
#include "game_state_internal.h"
#include "engine_internal.h"
#include "memory_arena.h"
#include "quadrant.h"
#include "quadrant_internal.h"
#include <stddef.h>

size_t grids_total_memory_size;

void init_grids(void) {
    
    size_t bigCount = engine.game->low_LOD_quadrant_count;
    size_t mediumCount = engine.game->medium_LOD_quadrant_count;
    size_t smallCount = engine.game->high_LOD_quadrant_count;
    //size_t unit_created_count = engine.game.unit_created_count;
    size_t number_of_grids = 3;

    grids_total_memory_size =
        sizeof(Grids) + (_Alignof(Grids) - 1) +
        sizeof(GridLowLOD) + (_Alignof(GridLowLOD) - 1) +
        sizeof(GridMediumLOD) + (_Alignof(GridMediumLOD) - 1) +
        sizeof(GridHighLOD) + (_Alignof(GridHighLOD) - 1) +
        (sizeof(Occupied) + (_Alignof(Occupied) - 1)) * number_of_grids +
        sizeof(*((Occupied *)0)->occupied_quads_indexes) * (bigCount + mediumCount + smallCount) +
        bigCount * sizeof(bigQuadrant) + (_Alignof(bigQuadrant) - 1) +
        mediumCount * sizeof(mediumQuadrant) + (_Alignof(mediumQuadrant) - 1) +
        smallCount * sizeof(smallQuadrant) + (_Alignof(smallQuadrant) - 1);
        
    if (memory_arena_memory_remainder() < grids_total_memory_size) {

        fprintf(stderr, "Grids memory allocation failed. Not enough memory available.\n");
        exit(EXIT_FAILURE);

    }

    engine.grid_memory_ptr = memory_arena_current_pointer();

    Grids *grids = memory_arena_push(sizeof(Grids), _Alignof(Grids));

    GridLowLOD *GLLoD = memory_arena_push(sizeof(GridLowLOD), _Alignof(GridLowLOD));

    GridMediumLOD *GMLoD = memory_arena_push(sizeof(GridMediumLOD), _Alignof(GridMediumLOD));

    GridHighLOD *GHLoD = memory_arena_push(sizeof(GridHighLOD), _Alignof(GridHighLOD));

    bigQuadrant *bigQuad = memory_arena_push(sizeof(bigQuadrant) * bigCount, _Alignof(bigQuadrant));
    
    mediumQuadrant *mediumQuad = memory_arena_push(sizeof(mediumQuadrant) * mediumCount, _Alignof(mediumQuadrant));
    
    smallQuadrant *smallQuad = memory_arena_push(sizeof(smallQuadrant) * smallCount, _Alignof(smallQuadrant));
    
    Occupied *occupied_GLLoD = memory_arena_push(sizeof(Occupied) + sizeof(unsigned int) * bigCount, _Alignof(Occupied));
    
    Occupied *occupied_GMLoD = memory_arena_push(sizeof(Occupied) + sizeof(unsigned int) * mediumCount, _Alignof(Occupied));
    
    Occupied *occupied_GHLoD = memory_arena_push(sizeof(Occupied) + sizeof(unsigned int) * smallCount, _Alignof(Occupied));
    
    engine.grids = grids;
    
    engine.grids->GLLoD = GLLoD;

    engine.grids->GLLoD->bigQuad = bigQuad;

    engine.grids->GLLoD->occupied = occupied_GLLoD;

    engine.grids->GMLoD = GMLoD;

    engine.grids->GMLoD->mediumQuad = mediumQuad;

    engine.grids->GMLoD->occupied = occupied_GMLoD;

    engine.grids->GHLoD = GHLoD;
    
    engine.grids->GHLoD->smallQuad = smallQuad;

    engine.grids->GHLoD->occupied = occupied_GHLoD;
   
    fill_quadrant_data();

}

void free_grid_memory(void) {
    
    memset(engine.grid_memory_ptr, 0, grids_total_memory_size);
    engine.grid_memory_ptr = NULL;
    
}