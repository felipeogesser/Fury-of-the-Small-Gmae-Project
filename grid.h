#ifndef GRID_H
#define GRID_H

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
} GridMediumLOD;

typedef struct GridHighLOD {
    struct smallQuadrant *smallQuad;
} GridHighLOD;

typedef struct Occupied {
    unsigned int quads_occupied_count;
    unsigned int index[];
} Occupied;

void init_grids(void);

void free_grid_memory(void);

#endif