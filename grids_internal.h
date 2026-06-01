#ifndef GRIDS_INTERNAL_H
#define GRIDS_INTERNAL_H

#include "quadrant_types.h"


typedef struct Grids {
    struct GridLowLOD *GLLoD;
    struct GridMediumLOD *GMLoD;
    struct GridHighLOD *GHLoD;
} Grids;

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
    unsigned int quads_occupied_count;
    unsigned int index[];
} Occupied;

#endif