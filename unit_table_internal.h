#ifndef UNIT_TABLE_INTERNAL_H
#define UNIT_TABLE_INTERNAL_H

#include "unit_table_types.h"
#include "animation_types.h"
#include "direction_types.h"
#include "target_types.h"

typedef struct UnitDynamicTable {

    enum Direction *direction;
    unsigned int *target_idx;
    enum TargetType *target_type;
    float *vec_x;
    float *vec_y;
    float *x;
    float *y;
    float *hp;
    float *attack_timer;
    enum Animation *animation;
    unsigned char *current_frame;
    _Bool *is_target_in_range;
    _Bool *is_engaged;
    _Bool *is_present;
    _Bool *is_charging;
    _Bool *is_attacking;

} UnitDynamicTable;

/*
// comparison table

typedef struct UnitComparisonTable {

    UnitDynamicTable friendly_dynamic, enemy_dynamic;

} UnitComparisonTable;

// mock tables

// mock dynamic tables

typedef struct MockUnitDynamicTable {

    enum Direction *direction;
    unsigned int *target_idx;
    enum TargetType *target_type;
    float *vec_x;
    float *vec_y;
    float *x;
    float *y;
    float *hp;
    enum Animation *animation;
    unsigned char *current_frame;
    _Bool *is_engaged;
    _Bool *is_alive;

} MockUnitDynamicTable;
*/
typedef struct UnitTable {
    
    UnitDynamicTable friendly_dynamic;
    UnitDynamicTable enemy_dynamic;

} UnitTable;

#endif