#ifndef ARMIES_INTERNAL_H
#define ARMIES_INTERNAL_H

#include "battalion_types.h"

typedef struct Armies {
    struct Army *army;
    unsigned int armies_count;
    unsigned int generals_screen_width;
    unsigned int generals_screen_height;
} Armies;

typedef struct Army {
    struct General *general;
    struct Battalion *battalions;
    unsigned int battalion_count;
    unsigned int general_count;
    unsigned int army_size;
    unsigned int unit_alive;
    unsigned int generals_screen_width;
    unsigned int generals_screen_height;
} Army;

#endif