#ifndef MAIN_MENU_INTERNAL_H
#define MAIN_MENU_INTERNAL_H

typedef struct MainMenu {

    unsigned char background_R_color;
    unsigned char background_G_color;
    unsigned char background_B_color;
    unsigned char background_Alpha;

    unsigned short button_battlefield_position_x;
    unsigned short button_battlefield_position_y;
    unsigned short button_battlefield_width_x;
    unsigned short button_battlefield_width_y;
    unsigned char button_battlefield_R_color;
    unsigned char button_battlefield_G_color;
    unsigned char button_battlefield_B_color;
    unsigned char button_battlefield_Alpha;
    const char *button_battlefield_text;

    unsigned short button_battleplan_position_x;
    unsigned short button_battleplan_position_y;
    unsigned short button_battleplan_width_x;
    unsigned short button_battleplan_width_y;
    unsigned char button_battleplan_R_color;
    unsigned char button_battleplan_G_color;
    unsigned char button_battleplan_B_color;
    unsigned char button_battleplan_Alpha;
    const char *button_battleplan_text;

} MainMenu;

#endif