#ifndef BATTALION_INTERNAL_H
#define BATTALION_INTERNAL_H

#include "battalion_types.h"
#include "fatigue_internal.h"
#include "general_types.h"
#include "health_pool.h"
#include "morale_internal.h"
#include "unit_types.h"

typedef struct BattalionState {
    enum BattalionLifecycle lifecycle;
    enum BattalionMovementMode movement;
    _Bool headless;
} BattalionState;

typedef struct Battalion {
    General *general;
    Unit *unit;
    BattalionState state;
    Morale morale;
    Fatigue fatigue;
    HealthPool health_pool;
    _Bool is_active;
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
} Battalion;

#endif