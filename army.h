#ifndef ARMY_H
#define ARMY_H

#include "battalion.h"

typedef struct Armies {
    struct Army *army;
} Armies;

typedef struct Army {
    struct Battalion *battalions;
    unsigned short battalion_count;

} Army;



#endif