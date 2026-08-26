#ifndef BATTLEPLAN_INTERNAL_H
#define BATTLEPLAN_INTERNAL_H

#include "battleplan_types.h"
#include "general_internal.h"

#define GRID_DIMENSION_X 5
#define GRID_DIMENSION_Y 6

typedef struct Background {

    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    // 32 bits

} Background;

typedef struct Button {

    unsigned short x;
    unsigned short y;
    unsigned short w;
    unsigned short h;
    unsigned char r;
    unsigned char g;
    unsigned char b;
    unsigned char a;
    const char *text;
    // 150 bits

} Button;

typedef struct DrawerSlot {

    General *general;
    float x;
    float y;
    float w;
    float h;

} DrawerSlot;

typedef struct Battleplan {

    Background background;
    Button button_main_menu;
    Button button_init_battle;
    General (*battleplan_general_placement)[GRID_DIMENSION_Y];
    unsigned int general_in_grid_count;
    
} Battleplan;
/*typedef struct Grid {

    General *general;

} Grid;*/

#endif