#include "grid.h"
#include "quadrant.h"

static void *grid_memory = NULL;
static Grids *grids = NULL;
void init_grids(void) {
    size_t bigCount = (size_t)game->low_LOD_quadrant_size;
    size_t medCount = (size_t)game->low_LOD_quadrant_size * 4;
    size_t smallCount = (size_t)game->low_LOD_quadrant_size * 16;

    size_t total =
        sizeof(Grids) +
        sizeof(GridLowLOD) + (alignof(GridLowLOD) - 1) +
        sizeof(GridMediumLOD) + (alignof(GridMediumLOD) - 1) +
        sizeof(GridHighLOD) + (alignof(GridHighLOD) - 1) +
        bigCount * sizeof(bigQuadrant) + (alignof(bigQuadrant) - 1) +
        medCount * sizeof(mediumQuadrant) + (alignof(mediumQuadrant) - 1) +
        smallCount * sizeof(smallQuadrant) + (alignof(smallQuadrant) - 1);
        

    grid_memory = calloc(1, total);

    char *p = grid_memory;
    
    grids = (Grids *)p;

    p += sizeof(Grids);

    p = (char*)(((uintptr_t)p + alignof(GridLowLOD) - 1) & ~(alignof(GridLowLOD) - 1));
    GridLowLOD *GLLoD = (GridLowLOD *)p;

    p += sizeof(GridLowLOD);

    p = (char*)(((uintptr_t)p + alignof(GridMediumLOD) - 1) & ~(alignof(GridMediumLOD) - 1));
    GridMediumLOD *GMLoD = (GridMediumLOD *)p;
    
    p += sizeof(GridMediumLOD);

    p = (char*)(((uintptr_t)p + alignof(GridHighLOD) - 1) & ~(alignof(GridHighLOD) - 1));
    GridHighLOD *GHLoD = (GridHighLOD *)p;

    p += sizeof(GridHighLOD);

    p = (char*)(((uintptr_t)p + alignof(bigQuadrant) - 1) & ~(alignof(bigQuadrant) - 1));
    bigQuadrant *bigQuad = (bigQuadrant *)p;
    
    p += bigCount * sizeof(bigQuadrant);

    p = (char*)(((uintptr_t)p + alignof(mediumQuadrant) - 1) & ~(alignof(mediumQuadrant) - 1));
    mediumQuadrant *mediumQuad = (mediumQuadrant *)p;

    p += medCount * sizeof(mediumQuadrant);

    p = (char*)(((uintptr_t)p + alignof(smallQuadrant) - 1) & ~(alignof(smallQuadrant) - 1));
    smallQuadrant *smallQuad = (smallQuadrant *)p;

    GLLoD->bigQuad = bigQuad;
    grids->GLLoD = GLLoD;
    GMLoD->mediumQuad = mediumQuad;
    grids->GMLoD = GMLoD;
    GHLoD->smallQuad = smallQuad;
    grids->GHLoD =GHLoD;

}
