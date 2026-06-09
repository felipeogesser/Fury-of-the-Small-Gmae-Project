#ifndef BATTLEPLAN_INTERNAL_H
#define BATTLEPLAN_INTERNAL_H

#include "battleplan_types.h"

typedef struct Battleplan {

    unsigned char background_R_color;
    unsigned char background_G_color;
    unsigned char background_B_color;
    unsigned char background_Alpha;

    unsigned short battleplan_button_position_X;
    unsigned short battleplan_button_position_Y;
    unsigned short battleplan_button_width_X;
    unsigned short battleplan_button_width_Y;
    unsigned char battleplan_button_R_color;
    unsigned char battleplan_button_G_color;
    unsigned char battleplan_button_B_color;
    unsigned char battleplan_button_Alpha;
    const char *battleplan_button_text;

} Battleplan;

#endif