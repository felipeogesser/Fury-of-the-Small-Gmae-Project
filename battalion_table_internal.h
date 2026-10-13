#ifndef BATTALION_TABLE_INTERNAL_H
#define BATTALION_TABLE_INTERNAL_H

#include "battalion_table_types.h"
#include "battalion_types.h"
#include "fatigue_types.h"
#include "morale_types.h"
#include "threat_types.h"

// static table

typedef struct BattalionStaticTable {

    const enum BattalionType *battalion_type;
    
    const float *range;
    const float *charge_bonus;
    const float *armor;
    const float *attack_speed;

    const unsigned int *start_entity_count;

    const unsigned char *formation_x;
    const unsigned char *formation_y;

    const _Bool *is_shielded;
    const _Bool *is_ranged;
    const _Bool *is_large;
    const _Bool *is_armor_piercing;
    const _Bool *is_anti_large;

} BattalionStaticTable;

typedef struct BattalionStaticTableDiff {

    const float *diff_armor;

} BattalionStaticTableDiff;

// dynamic tables

typedef struct BattalionDynamicTable {

    //enum ThreatLevel *threat;
    enum HeadlessMovementMode *headless_movement_mode;
    enum FatigueState *fatigue_state;
    enum MoraleState *morale_state;
    enum BattalionLifecycle *lifecycle_state;
    enum BattalionMovementMode *movement_state;
    enum BattalionMovementPace *movement_pace;
    unsigned int *target_battalion_idx;
    unsigned int *morale_modifiers;
    unsigned int *withdrawed_entity_count;
    unsigned int *dead_entity_count;
    unsigned int *alive_entity_count;
    unsigned int *unit_alive_count;
    //float *order_x;
    //float *order_y;
    float *target_x;
    float *target_y;
    //float *anchor_x;
    //float *anchor_y;
    float *x;
    float *y;
    float *morale;
    float *fatigue;
    float *hp;
    float *speed;
    float *meele_attack;
    float *meele_defense;
    float *strength;
    float *rally_timer;
    float *charge_timer;
    
    _Bool *is_active;
    _Bool *is_rallying;
    _Bool *is_charging;
    _Bool *is_engaging;
    _Bool *is_engaged;
    //_Bool *is_routing;
    _Bool *is_controllable;
    _Bool *is_present;
    _Bool *is_headless;
    _Bool *is_bracing;
    //_Bool *is_attacking;

} BattalionDynamicTable;

typedef struct BattalionDynamicTableDiff {

    enum ThreatLevel *threat;
    float *dx;
    float *dy;
    float *distance;
    float *diff_morale;
    float *diff_fatigue;
    float *diff_hp;
    float *diff_speed;
    float *diff_meele_attack;
    float *diff_meele_defense;
    float *diff_strength;

} BattalionDynamicTableDiff;
/*
// comparison table

typedef struct BattalionComparisonTable {

    BattalionStaticTable friendly_static, enemy_static;
    BattalionStaticTableDiff friendly_static_diff;
    BattalionStaticTableDiff enemy_static_diff;

    BattalionDynamicTable friendly_dynamic, enemy_dynamic;
    BattalionDynamicTableDiff friendly_dynamic_diff;
    BattalionDynamicTableDiff enemy_dynamic_diff;

} BattalionComparisonTable;*/

/*
// mock tables

// mock static tables

typedef struct BattalionStaticTable {

    enum BattalionType *battalion_type;
    
    float *range;
    float *charge_bonus;
    float *armor;
    float *attack_speed;

    unsigned int *start_entity_count;

    unsigned char *formation_x;
    unsigned char *formation_y;

    _Bool *is_shielded;
    _Bool *is_ranged;
    _Bool *is_large;
    _Bool *is_armor_piercing;
    _Bool *is_anti_large;

} BattalionStaticTable;

typedef struct BattalionStaticTableDiff {

    float *diff_armor;

} BattalionStaticTableDiff;

// mock dynamic tables

typedef struct BattalionDynamicTable {

    //enum ThreatLevel *threat;
    enum HeadlessMovementMode *headless_movement_mode;
    enum FatigueState *fatigue_state;
    enum MoraleState *morale_state;
    enum BattalionLifecycle *lifecycle_state;
    enum BattalionMovementMode *movement_state;
    enum BattalionMovementPace *movement_pace;
    unsigned int *target_battalion_idx;
    unsigned int *morale_modifiers;
    unsigned int *withdrawed_entity_count;
    unsigned int *dead_entity_count;
    unsigned int *alive_entity_count;
    unsigned int *unit_alive_count;
    float *order_x;
    float *order_y;
    float *target_x;
    float *target_y;
    //float *anchor_x;
    //float *anchor_y;
    float *x;
    float *y;
    float *morale;
    float *fatigue;
    float *hp;
    float *speed;
    float *meele_attack;
    float *meele_defense;
    float *strength;
    float *rally_timer;
    float *charge_timer;
    
    _Bool *is_active;
    _Bool *is_rallying;
    _Bool *is_charging;
    _Bool *is_engaging;
    _Bool *is_engaged;
    //_Bool *is_routing;
    _Bool *is_controllable;
    _Bool *is_present;
    _Bool *is_headless;
    _Bool *is_bracing;
    //_Bool *is_attacking;

} BattalionDynamicTable;

typedef struct BattalionDynamicTableDiff {

    enum ThreatLevel *threat;
    float *dx;
    float *dy;
    float *distance;
    float *diff_morale;
    float *diff_fatigue;
    float *diff_hp;
    float *diff_speed;
    float *diff_meele_attack;
    float *diff_meele_defense;
    float *diff_strength;

} BattalionDynamicTableDiff;
*/

typedef struct BattalionTable {

    BattalionStaticTable friendly_static;
    BattalionStaticTable enemy_static;
    BattalionDynamicTable friendly_dynamic;
    BattalionDynamicTable enemy_dynamic;

    BattalionStaticTableDiff friendly_static_diff;
    BattalionDynamicTableDiff friendly_dynamic_diff;

} BattalionTable;

#endif