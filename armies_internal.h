#ifndef ARMIES_INTERNAL_H
#define ARMIES_INTERNAL_H

#include "battalion_types.h"

typedef struct Armies {
    struct Army *army;
    unsigned int number_of_armies;
} Armies;

typedef struct Army {
    struct Battalion *battalions;
    unsigned int battalion_count;
    unsigned int army_size;
    unsigned int entities_alive;
} Army;

#endif