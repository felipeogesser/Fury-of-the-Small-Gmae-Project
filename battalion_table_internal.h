#ifndef BATTALION_TABLE_INTERNAL_H
#define BATTALION_TABLE_INTERNAL_H

#include "battalion_table_types.h"
#include "battalion_types.h"
#include "fatigue_types.h"
#include "morale_types.h"
#include "threat_types.h"

// static table

typedef struct BattalionStaticTable {

    const enum BattalionType *const battalion_type;
    
    const float *const range;
    const float *const charge_bonus;
    const float *const armor;
    const float *const attack_speed;

    const unsigned int *const start_entity_count;

    const unsigned char *const formation_x;
    const unsigned char *const formation_y;

    const _Bool *const shielded;
    const _Bool *const ranged;
    const _Bool *const large;
    const _Bool *const armor_piercing;
    const _Bool *const anti_large;

} BattalionStaticTable;

typedef struct BattalionStaticTableDiff {

    const float *const diff_armor;

} BattalionStaticTableDiff;

// dynamic tables

typedef struct BattalionDynamicTable {

    //enum ThreatLevel *threat;
    enum HeadlessMovementMode *const headless_movement_mode;
    enum FatigueState *const fatigue_state;
    enum MoraleState *const morale_state;
    enum BattalionLifecycle *const lifecycle_state;
    enum BattalionMovementMode *const movement_state;
    enum BattalionMovementPace *const movement_pace;
    unsigned int *const target_battalion_idx;
    unsigned int *const morale_modifiers;
    unsigned int *const withdrawed_entity_count;
    unsigned int *const dead_entity_count;
    unsigned int *const alive_entity_count;
    unsigned int *const unit_alive_count;
    //float *const order_x;
    //float *const order_y;
    float *const target_x;
    float *const target_y;
    //float *const anchor_x;
    //float *const anchor_y;
    float *const x;
    float *const y;
    float *const morale;
    float *const fatigue;
    float *const hp;
    float *const speed;
    float *const meele_attack;
    float *const meele_defense;
    float *const strength;
    float *const rally_timer;
    float *const charge_timer;
    
    _Bool *const is_active;
    _Bool *const is_rallying;
    _Bool *const is_charging;
    _Bool *const is_engaging;
    _Bool *const is_engaged;
    //_Bool *const is_routing;
    _Bool *const is_controllable;
    _Bool *const is_present;
    _Bool *const is_headless;
    _Bool *const is_bracing;
    //_Bool *const is_attacking;

} BattalionDynamicTable;

typedef struct BattalionDynamicTableDiff {

    enum ThreatLevel *const threat;
    float *const dx;
    float *const dy;
    float *const distance;
    float *const diff_morale;
    float *const diff_fatigue;
    float *const diff_hp;
    float *const diff_speed;
    float *const diff_meele_attack;
    float *const diff_meele_defense;
    float *const diff_strength;

} BattalionDynamicTableDiff;

// comparison table

typedef struct BattalionComparisonTable {

    BattalionStaticTable friendly_static, enemy_static;
    BattalionStaticTableDiff friendly_static_diff;
    BattalionStaticTableDiff enemy_static_diff;

    BattalionDynamicTable friendly_dynamic, enemy_dynamic;
    BattalionDynamicTableDiff friendly_dynamic_diff;
    BattalionDynamicTableDiff enemy_dynamic_diff;

} BattalionComparisonTable;

// mock tables

// mock static tables

typedef struct MockBattalionStaticTable {

    enum BattalionType *battalion_type;
    
    float *range;
    float *charge_bonus;
    float *armor;
    float *attack_speed;

    unsigned int *start_entity_count;

    unsigned char *formation_x;
    unsigned char *formation_y;

    _Bool *shielded;
    _Bool *ranged;
    _Bool *large;
    _Bool *armor_piercing;
    _Bool *anti_large;

} MockBattalionStaticTable;

typedef struct MockBattalionStaticTableDiff {

    float *diff_armor;

} MockBattalionStaticTableDiff;

// mock dynamic tables

typedef struct MockBattalionDynamicTable {


    //enum ThreatLevel *threat;
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
    float *order_x;
    float *order_y;
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
    
    _Bool *is_active;
    _Bool *is_rallying;
    _Bool *is_charging;
    _Bool *is_engaging;
    _Bool *is_engaged;
    //_Bool *const is_routing;
    _Bool *is_controllable;
    _Bool *is_present;
    _Bool *is_headless;
    _Bool *is_bracing;
    //_Bool *const is_attacking;

} MockBattalionDynamicTable;

typedef struct MockBattalionDynamicTableDiff {

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
} MockBattalionDynamicTableDiff;


typedef struct BattalionTable {

    BattalionStaticTable friendly_static;
    BattalionStaticTable enemy_static;
    BattalionDynamicTable friendly_dynamic;
    BattalionDynamicTable enemy_dynamic;

    BattalionStaticTableDiff friendly_static_diff;
    BattalionDynamicTableDiff friendly_dynamic_diff;

} BattalionTable;

#endif