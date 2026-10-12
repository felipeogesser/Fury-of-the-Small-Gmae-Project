#ifndef BATTALION_INTERNAL_H
#define BATTALION_INTERNAL_H

#include "battalion_types.h"
#include "fatigue_internal.h"
#include "general_types.h"
#include "health_pool.h"
#include "morale_internal.h"
#include "position.h"
#include "sprites_types.h"
#include "unit_types.h"

typedef struct Rally {

    float timer;
    _Bool rallying;

} Rally;

typedef struct BattalionState {

    enum BattalionLifecycle lifecycle;
    enum BattalionMovementMode movement;
    Rally rally;
    _Bool headless;
    
} BattalionState;

typedef struct BattalionStatistics {

    unsigned int unit_base_count;
    unsigned int unit_dead_count;
    unsigned int unit_withdrawed_count;

} BattalionStatistics;

typedef struct Battalion {
    General *general;
    Unit *unit;
    BattalionState state;
    Morale morale;
    Fatigue fatigue;
    HealthPool health_pool;
    BattalionStatistics statistics;
    _Bool is_active;
    Position pos; // avg pos of all units
    enum BattalionType battalion_type;
    enum Sprites sprite_type;
    unsigned int unit_count;
    unsigned int padding_between_units_x;
    unsigned int padding_between_units_y;
    unsigned int unit_formation_width;
    unsigned int unit_formation_height;
    float unit_screen_width;
    float unit_screen_height;
    unsigned char R_Color;
    unsigned char G_Color;
    unsigned char B_Color;
    unsigned char Alpha;
    float speed;
    float meele_attack;
    float meele_defense;
    float strength;
    float attack_speed;
    float range;
    float charge_bonus;
    float armor;
    _Bool is_shielded;
    _Bool is_ranged;
    _Bool is_large;
    _Bool is_armor_piercing;
    _Bool is_anti_large;
    _Bool is_routing;
    _Bool is_controllable;
    _Bool is_present;

    unsigned int unit_alive_count;
    unsigned int start_entity_count;
    unsigned int formation_x;
    unsigned int formation_y;

} Battalion;

#endif