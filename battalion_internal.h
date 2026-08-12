#ifndef BATTALION_INTERNAL_H
#define BATTALION_INTERNAL_H

#include "unit_types.h"

typedef struct Battalion {
    struct General *general;
    struct Unit *unit;
    unsigned int unit_count;
    float unit_screen_width;
    float unit_screen_height;
    unsigned char R_Color;
    unsigned char G_Color;
    unsigned char B_Color;
    unsigned char Alpha;
} Battalion;

#endif