#ifndef BATTLEPLAN_INTERNAL_H
#define BATTLEPLAN_INTERNAL_H

#include "battleplan_types.h"

typedef struct Battleplan {

    unsigned char background_R_color;
    unsigned char background_G_color;
    unsigned char background_B_color;
    unsigned char background_Alpha;

    unsigned short button_main_menu_position_x;
    unsigned short button_main_menu_position_y;
    unsigned short button_main_menu_width_x;
    unsigned short button_main_menu_width_y;
    unsigned char button_main_menu_R_color;
    unsigned char button_main_menu_G_color;
    unsigned char button_main_menu_B_color;
    unsigned char button_main_menu_Alpha;
    const char *button_main_menu_text;

} Battleplan;

#endif