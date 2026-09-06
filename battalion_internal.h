#ifndef BATTALION_INTERNAL_H
#define BATTALION_INTERNAL_H

#include "battalion_types.h"
#include "field_entry.h"
#include "unit_types.h"

typedef struct Battalion {
    struct General *general;
    struct Unit *unit;
    unsigned int unit_count;
    //unsigned int initial_map_placement_x;
    //unsigned int initial_map_placement_y;
    unsigned int padding_between_units_x;
    unsigned int padding_between_units_y;
    unsigned int unit_formation_width;
    unsigned int unit_formation_height;
    unsigned int area_width;
    unsigned int area_height;
    float unit_screen_width;
    float unit_screen_height;
    unsigned char R_Color;
    unsigned char G_Color;
    unsigned char B_Color;
    unsigned char Alpha;
} Battalion;

extern const FieldEntry battalion_field_table[];
extern const size_t battalion_field_table_count;
#endif