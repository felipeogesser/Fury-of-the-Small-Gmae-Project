#include "grid.h"
#include "quadrant.h"

static void *grid_memory = NULL;
static Grids *grids = NULL;
void init_grids(void) {
    size_t bigCount = (size_t)game->low_LOD_quadrant_size;
    size_t mediumCount = (size_t)game->medium_LOD_quadrant_size;
    size_t smallCount = (size_t)game->high_LOD_quadrant_size;
    size_t number_of_grids = 3;
    size_t total =
        sizeof(Grids) +
        sizeof(GridLowLOD) + (_Alignof(GridLowLOD) - 1) +
        sizeof(GridMediumLOD) + (_Alignof(GridMediumLOD) - 1) +
        sizeof(GridHighLOD) + (_Alignof(GridHighLOD) - 1) +
        (sizeof(Occupied) + (_Alignof(Occupied) - 1)) * number_of_grids +
        sizeof(*((Occupied *)0)->index) * (bigCount + mediumCount + smallCount) +
        bigCount * sizeof(bigQuadrant) + (_Alignof(bigQuadrant) - 1) +
        mediumCount * sizeof(mediumQuadrant) + (_Alignof(mediumQuadrant) - 1) +
        smallCount * sizeof(smallQuadrant) + (_Alignof(smallQuadrant) - 1);
        

    grid_memory = calloc(1, total);

    char *p = grid_memory;
    
    grids = (Grids *)p;

    p += sizeof(Grids);

    p = (char *)(((uintptr_t)p + _Alignof(GridLowLOD) - 1) & ~(_Alignof(GridLowLOD) - 1));
    grids->GLLoD = (GridLowLOD *)p;

    p += sizeof(GridLowLOD);

    p = (char *)(((uintptr_t)p + _Alignof(Occupied) - 1) & ~(_Alignof(Occupied) - 1));
    grids->GLLoD->occupied = (Occupied *)p;

    p += sizeof(Occupied);

    grids->GLLoD->occupied->index = (void *)p;
    p += sizeof(*((Occupied *)0)->index) * bigCount;

    p = (char *)(((uintptr_t)p + _Alignof(GridMediumLOD) - 1) & ~(_Alignof(GridMediumLOD) - 1));
    grids->GMLoD = (GridMediumLOD *)p;
    
    p += sizeof(GridMediumLOD);

    p = (char *)(((uintptr_t)p + _Alignof(Occupied) - 1) & ~(_Alignof(Occupied) - 1));
    grids->GMLoD->occupied = (Occupied *)p;

    p += sizeof(Occupied);

    p = (char *)(((uintptr_t)p + _Alignof(*((Occupied *)0)->index) - 1) & ~(_Alignof(*((Occupied *)0)->index) - 1));

    grids->GMLoD->occupied->index = (void *)p;
    p += sizeof(*((Occupied *)0)->index) * mediumCount;

    p = (char *)(((uintptr_t)p + _Alignof(GridHighLOD) - 1) & ~(_Alignof(GridHighLOD) - 1));
    grids->GHLoD = (GridHighLOD *)p;

    p += sizeof(GridHighLOD);

    p = (char *)(((uintptr_t)p + _Alignof(Occupied) - 1) & ~(_Alignof(Occupied) - 1));
    grids->GHLoD->occupied = (Occupied *)p;

    p += sizeof(Occupied);

    grids->GHLoD->occupied->index = (void *)p; 
    p += sizeof(*((Occupied *)0)->index) * smallCount;


    p = (char *)(((uintptr_t)p + _Alignof(bigQuadrant) - 1) & ~(_Alignof(bigQuadrant) - 1));
    grids->GLLoD->bigQuad = (bigQuadrant *)p;
    
    p += sizeof(bigQuadrant) * bigCount;

    p = (char *)(((uintptr_t)p + _Alignof(mediumQuadrant) - 1) & ~(_Alignof(mediumQuadrant) - 1));
    grids->GMLoD->mediumQuad = (mediumQuadrant *)p;

    p += sizeof(mediumQuadrant) * mediumCount;

    p = (char *)(((uintptr_t)p + _Alignof(smallQuadrant) - 1) & ~(_Alignof(smallQuadrant) - 1));
    grids->GHLoD->smallQuad = (smallQuadrant *)p;

}

void free_grid_memory(void) {
    free(grid_memory);
    grid_memory = NULL;
}