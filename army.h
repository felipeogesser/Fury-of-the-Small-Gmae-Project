#ifndef ARMY_H
#define ARMY_H

#include "battalion.h"

typedef struct Armies {
    struct Army *army;
    unsigned int number_of_armies;
} Armies;

typedef struct Army {
    struct Battalion *battalions;
    unsigned short battalion_count;
    unsigned int army_size;
    unsigned int entities_alive;
} Army;



#endif