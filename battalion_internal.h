#ifndef BATTALION_INTERNAL_H
#define BATTALION_INTERNAL_H

#include "battalion_types.h"
#include "general_types.h"
#include "unit_types.h"

typedef struct Battalion {
    General *general;
    Unit *unit;
    unsigned int unit_count;
    unsigned int padding_between_units_x;
    unsigned int padding_between_units_y;
    unsigned int unit_formation_width;
    unsigned int unit_formation_height;
    float unit_screen_width;
    float unit_screen_height;
    unsigned char R_Color;
    unsigned char G_Color;
    unsigned char B_Color;
    unsigned char Alpha;
} Battalion;

#endif