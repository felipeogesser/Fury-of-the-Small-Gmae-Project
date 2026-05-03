#ifndef GRID_H
#define GRID_H

typedef struct Grids {
    struct GridLowLOD *GLLoD;
    struct GridMediumLOD *GMLoD;
    struct GridHighLOD *GHLoD;
} Grids;

typedef struct Occupied {
    unsigned int occupied_count;
    unsigned int index[quantity / 16];
} Occupied;

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

#endif