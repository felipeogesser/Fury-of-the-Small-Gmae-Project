#ifndef MORALE_INTERNAL_H
#define MORALE_INTERNAL_H

#include "morale_types.h"

typedef struct Morale {

    enum MoraleState state;
    signed int current;
    unsigned int base;
    unsigned int modifiers;

} Morale;

#endif