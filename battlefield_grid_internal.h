#ifndef BATTLEFIELD_GRID_INTERNAL_H
#define BATTLEFIELD_GRID_INTERNAL_H

#include "battlefield_grid_types.h"
#include "quadrant_types.h"

typedef struct BattlefieldGrid {

    struct GridLowLOD *GLLoD;
    struct GridMediumLOD *GMLoD;
    struct GridHighLOD *GHLoD;

} BattlefieldGrid;

typedef struct GridLowLOD {

    struct bigQuadrant *bigQuad;
    struct Occupied *occupied;

} GridLowLOD;

typedef struct GridMediumLOD {

    struct mediumQuadrant *mediumQuad;
    struct Occupied *occupied;

} GridMediumLOD;

typedef struct GridHighLOD {

    struct smallQuadrant *smallQuad;
    struct Occupied *occupied;

} GridHighLOD;

typedef struct Occupied {

    unsigned int occupied_quads_count;
    unsigned int occupied_quads_indexes[];

} Occupied;

#endif