#ifndef FATIGUE_TYPES_H
#define FATIGUE_TYPES_H

typedef struct Fatigue Fatigue;

enum FatigueState {

    FRESH = 0,
    ACTIVE,
    WINDED,
    TIRED,
    VERY_TIRED,
    EXHAUSTED,
    FATIGUE_STATE_COUNT

};

#endif