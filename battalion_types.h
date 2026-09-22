#ifndef BATTALION_TYPES_H
#define BATTALION_TYPES_H

#include "field_entry_types.h"

typedef struct Battalion Battalion;

extern const FieldEntry *const battalion_field_table;

// Battalion AI policies
enum BattalionFLags {

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

};

#endif