#ifndef GRID_H
#define GRID_H

#include "quadrant.h"
#include <stddef.h>

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

extern size_t grids_total;

void init_grids(void);

void free_grid_memory(void);

#endif