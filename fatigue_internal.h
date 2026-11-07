#ifndef FATIGUE_INTERNAL_H
#define FATIGUE_INTERNAL_H

#include "fatigue_types.h"

typedef struct Fatigue {

    enum FatigueState state;
    unsigned int base;
    float current;

} Fatigue;

#endif