#ifndef BATTALION_TYPES_H
#define BATTALION_TYPES_H

#include "field_entry_types.h"

#define RALLY_TIMER 5.0f // 5 seconds

typedef struct Battalion Battalion;

extern const ComplexFieldEntry *const battalion_field_table_ptr;

// Battalion AI policies
/*enum BattalionPolicies {

    STATIONARY = (1 << 0),
    WALKING = (1 << 1),
    RUNNING = (1 << 2),
    CHARGING = (1 << 3),
    ENGAGED = (1 << 4),
    BRACING = (1 << 5),
    HEADLESS = (1 << 6),
    ROUTING = (1 << 7),
    WITHDRAWN = (1 << 8),
    WIPED_OUT = (1 << 9)

};*/

enum BattalionType {

    BATTALION_TYPE_UNKNOWN = -1,
    TEMPLAR,
    CAVALRY,
    GUARDSMEN,
    SWORDSMEN,
    ARCHERS,
    BATTALION_TYPE_COUNT = 32

};

enum BattalionLifecycle {

    UNKWON_LIFECYCLE = -1,
    NORMAL,
    ROUTING,
    WITHDRAWN,
    WIPED_OUT

};

enum BattalionMovementMode {

    UNKNOWN_MOVEMENT = -1,
    STATIONARY,
    WALKING,
    RUNNING,
    CHARGING,
    FIGHTING

};

enum BattalionMovementPace {

    UNKNOWN_PACE = -1,
    PACE_WALK,
    PACE_RUN

};

#endif