#include "grid.h"
#include "quadrant.h"

static void *gridMemory = NULL;
unsigned int quantity;
short smallQuadrantSize = 64;





Grids *grids = NULL;

void calculateAmountOfQuadrants(Map *map, short smallQuadrantSize) {
    amountX = map->mapSizeX / smallQuadrantSize;
    amountY = map->mapSizeY / smallQuadrantSize;
    quantity = amountX * amountY;
}

void init_grids(void) {
    size_t bigCount = quantity / 16;
    size_t medCount = quantity / 4;
    size_t smallCount = quantity;

    size_t total =
        bigCount * sizeof(bigQuadrant) +
        medCount * sizeof(mediumQuadrant) +
        smallCount * sizeof(smallQuadrant) +
        sizeof(GridLowLOD) + sizeof(GridMediumLOD) +
        sizeof(GridHighLOD) + sizeof(Grids);

    gridMemory = calloc(1, total);

    char *p = gridMemory;
    
    grids = (Grids *)p;

    p += sizeof(Grids);

    GridLowLOD *GLLoD = (GridLowLOD *)p;

    p += sizeof(GridLowLOD);

    bigQuadrant *bigQuad = (bigQuadrant *)p;
    
    p += bigCount * sizeof(bigQuadrant);
    
    GridMediumLOD *GMLoD = (GridMediumLOD *)p;
    
    p += sizeof(GridMediumLOD);

    mediumQuadrant *mediumQuad = (mediumQuadrant *)p;

    p += medCount * sizeof(mediumQuadrant);
    
    GridHighLOD *GHLoD = (GridHighLOD *)p;

    p += sizeof(GridHighLOD);

    smallQuadrant *smallQuad = (smallQuadrant *)p;


    GLLoD->bigQuad = bigQuad;
    grids->GLLoD = GLLoD;
    GMLoD->mediumQuad = mediumQuad;
    grids->GMLoD = GMLoD;
    GHLoD->smallQuad = smallQuad;
    grids->GHLoD =GHLoD;

}








