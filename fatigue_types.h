#ifndef FATIGUE_TYPES_H
#define FATIGUE_TYPES_H

#include "field_entry_internal.h"

typedef struct Fatigue Fatigue;

enum FatigueState {

    FATIGUE_UNKNOWN = -1,
    FRESH,
    ACTIVE,
    WINDED,
    TIRED,
    VERY_TIRED,
    EXHAUSTED,
    FATIGUE_STATE_COUNT

};

extern const ComplexFieldEntry fatigue_field_table[];

#endif