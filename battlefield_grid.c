#include "battlefield_grid.h"
#include "battlefield_grid_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "memory_arena.h"
#include "quadrant.h"
#include "quadrant_internal.h"
#include <stddef.h>

size_t grids_total_memory_size;

void init_grids(void) {
    
    size_t bigCount = engine.game->low_LOD_quadrant_count;
    size_t mediumCount = engine.game->medium_LOD_quadrant_count;
    size_t smallCount = engine.game->high_LOD_quadrant_count;
    size_t number_of_grids = 3;

    grids_total_memory_size =
        sizeof(BattlefieldGrid) + (_Alignof(BattlefieldGrid) - 1) +
        sizeof(GridLowLOD) + (_Alignof(GridLowLOD) - 1) +
        sizeof(GridMediumLOD) + (_Alignof(GridMediumLOD) - 1) +
        sizeof(GridHighLOD) + (_Alignof(GridHighLOD) - 1) +
        (sizeof(Occupied) + (_Alignof(Occupied) - 1)) * number_of_grids +
        sizeof(*((Occupied *)0)->occupied_quads_indexes) * (bigCount + mediumCount + smallCount) +
        bigCount * sizeof(bigQuadrant) + (_Alignof(bigQuadrant) - 1) +
        mediumCount * sizeof(mediumQuadrant) + (_Alignof(mediumQuadrant) - 1) +
        smallCount * sizeof(smallQuadrant) + (_Alignof(smallQuadrant) - 1);
        
    if (memory_arena_memory_remainder() < grids_total_memory_size) {

        fprintf(stderr, "BattlefieldGrid memory allocation failed. Not enough memory available.\n");
        exit(EXIT_FAILURE);

    }

    engine.grid_memory_ptr = memory_arena_current_pointer();

    BattlefieldGrid *battlefield_grid = memory_arena_push(sizeof(BattlefieldGrid), _Alignof(BattlefieldGrid));

    GridLowLOD *GLLoD = memory_arena_push(sizeof(GridLowLOD), _Alignof(GridLowLOD));

    GridMediumLOD *GMLoD = memory_arena_push(sizeof(GridMediumLOD), _Alignof(GridMediumLOD));

    GridHighLOD *GHLoD = memory_arena_push(sizeof(GridHighLOD), _Alignof(GridHighLOD));

    bigQuadrant *bigQuad = memory_arena_push(sizeof(bigQuadrant) * bigCount, _Alignof(bigQuadrant));
    
    mediumQuadrant *mediumQuad = memory_arena_push(sizeof(mediumQuadrant) * mediumCount, _Alignof(mediumQuadrant));
    
    smallQuadrant *smallQuad = memory_arena_push(sizeof(smallQuadrant) * smallCount, _Alignof(smallQuadrant));
    
    Occupied *occupied_GLLoD = memory_arena_push(sizeof(Occupied) + sizeof(unsigned int) * bigCount, _Alignof(Occupied));
    
    Occupied *occupied_GMLoD = memory_arena_push(sizeof(Occupied) + sizeof(unsigned int) * mediumCount, _Alignof(Occupied));
    
    Occupied *occupied_GHLoD = memory_arena_push(sizeof(Occupied) + sizeof(unsigned int) * smallCount, _Alignof(Occupied));
    
    engine.battlefield_grid = battlefield_grid;
    
    engine.battlefield_grid->GLLoD = GLLoD;

    engine.battlefield_grid->GLLoD->bigQuad = bigQuad;

    engine.battlefield_grid->GLLoD->occupied = occupied_GLLoD;

    engine.battlefield_grid->GMLoD = GMLoD;

    engine.battlefield_grid->GMLoD->mediumQuad = mediumQuad;

    engine.battlefield_grid->GMLoD->occupied = occupied_GMLoD;

    engine.battlefield_grid->GHLoD = GHLoD;
    
    engine.battlefield_grid->GHLoD->smallQuad = smallQuad;

    engine.battlefield_grid->GHLoD->occupied = occupied_GHLoD;
   
    fill_quadrant_data();

}

void free_grid_memory(void) {
    
    memset(engine.grid_memory_ptr, 0, grids_total_memory_size);
    engine.grid_memory_ptr = NULL;
    
}