#ifndef ARMY_H
#define ARMY_H

#include "battalion.h"

typedef struct Army {
    struct Battalion *Battalions;
    unsigned short battalion_count;

} Army;



#endif