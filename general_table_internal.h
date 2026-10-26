#ifndef GENERAL_TABLE_INTERNAL_H
#define GENERAL_TABLE_INTERNAL_H

#include "general_table_types.h"
#include "animation_types.h"
#include "direction_types.h"
#include "sprites_types.h"
#include "target_types.h"

// static table

typedef struct GeneralStaticTable {
    
    float *base_meele_attack;
    float *base_meele_defense;
    float *base_strength;
    float *range;
    float *charge_bonus;
    float *armor;
    float *attack_speed;

    enum Sprites *sprite_type;

    _Bool *is_shielded;
    _Bool *is_ranged;
    _Bool *is_large;
    _Bool *is_armor_piercing;
    _Bool *is_anti_large;

} GeneralStaticTable;

/*typedef struct GeneralStaticTableDiff {

    float *diff_armor;

} GeneralStaticTableDiff;*/

// dynamic tables

typedef struct GeneralDynamicTable {

    enum Direction *direction;
    enum TargetType *target_type;
    unsigned int *personal_target_idx;
    enum Animation *animation;
    unsigned char *current_frame;
    float *order_x;
    float *order_y;
    float *vec_x;
    float *vec_y;
    float *x;
    float *y;
    float *hp;
    float *meele_attack;
    float *meele_defense;
    float *strength;
    float *attack_timer;
    //_Bool *const is_routing;
    //_Bool *const controllable;
    float *alpha;
    _Bool *is_dead;
    _Bool *is_target_in_range;
    //_Bool *collision_with_enemy;
    // status
    _Bool *is_present;
    _Bool *is_engaged;
    _Bool *is_engaging;
    _Bool *is_charging;

} GeneralDynamicTable;

/*typedef struct GeneralDynamicTableDiff {

    float *dx;
    float *dy;
    float *distance;
    float *diff_hp;
    float *diff_meele_attack;
    float *diff_meele_defense;
    float *diff_strength;

} GeneralDynamicTableDiff;*/
/*
// comparison table

typedef struct GeneralComparisonTable {

    GeneralStaticTable friendly_static, enemy_static;
    GeneralStaticTableDiff friendly_static_diff;
    GeneralStaticTableDiff enemy_static_diff;

    GeneralDynamicTable friendly_dynamic, enemy_dynamic;
    GeneralDynamicTableDiff friendly_dynamic_diff;
    GeneralDynamicTableDiff enemy_dynamic_diff;

} GeneralComparisonTable;

// mock tables

// mock static tables

typedef struct MockGeneralStaticTable {

    float *base_meele_attack;
    float *base_meele_defense;
    float *range;
    float *charge_bonus;
    float *armor;
    float *attack_speed;

    _Bool *is_shielded;
    _Bool *is_ranged;
    _Bool *is_large;
    _Bool *is_armor_piercing;
    _Bool *is_anti_large;

} MockGeneralStaticTable;

typedef struct MockGeneralStaticTableDiff {

    float *diff_armor;

} MockGeneralStaticTableDiff;

// mock dynamic tables

typedef struct MockGeneralDynamicTable {
    
    enum Direction *direction;

    enum TargetType *target_type;
    unsigned int *personal_target_idx;
    enum Animation *animation;
    unsigned char *current_frame;
    float *vec_x;
    float *vec_y;
    float *x;
    float *y;
    float *hp;
    float *meele_attack;
    float *meele_defense;
    float *strength;
    float *attack_timer;

    //                                                                 _Bool *is_routing;
    //_Bool *controllable;
    _Bool *is_target_in_range;
    _Bool *is_present;
    _Bool *is_engaged;
    _Bool *is_engaging;

} MockGeneralDynamicTable;

typedef struct MockGeneralDynamicTableDiff {

    float *dx;
    float *dy;
    float *distance;
    float *diff_hp;
    float *diff_meele_attack;
    float *diff_meele_defense;
    float *diff_strength;

} MockGeneralDynamicTableDiff;
*/
typedef struct GeneralTable {

    GeneralStaticTable friendly_static;
    GeneralStaticTable enemy_static;
    GeneralDynamicTable friendly_dynamic;
    GeneralDynamicTable enemy_dynamic;

} GeneralTable;

#endif