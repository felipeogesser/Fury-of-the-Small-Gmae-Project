#ifndef BATTLEPLAN_GRID_INTERNAL_H
#define BATTLEPLAN_GRID_INTERNAL_H

#include "battleplan_grid_types.h"
#include "general_internal.h"

typedef struct Dimension {

    unsigned int x, y;

} Dimension;

typedef struct BattleplanGrid {

    //Cell cell;
    
    Dimension dimension;
    //Size size;

} BattleplanGrid;

typedef struct OccupiedCell {

    unsigned int x;
    unsigned int y;
    General general;

} OccupiedCell;

typedef struct GridPlacementPayload {

    BattleplanGrid grid;
    unsigned int occupied_cell_count;
    OccupiedCell occupied_cell[];

} GridPlacementPayload;

#endif