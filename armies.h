#ifndef ARMIES_H
#define ARMIES_H

#include "battalion.h"

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