#ifndef MAIN_MENU_INTERNAL_H
#define MAIN_MENU_INTERNAL_H

typedef struct MainMenu {

    unsigned char background_R_color;
    unsigned char background_G_color;
    unsigned char background_B_color;
    unsigned char background_Alpha;

    unsigned short start_button_position_X;
    unsigned short start_button_position_Y;
    unsigned short start_button_width_X;
    unsigned short start_button_width_Y;
    unsigned char start_button_R_color;
    unsigned char start_button_G_color;
    unsigned char start_button_B_color;
    unsigned char start_button_Alpha;
    const char *start_button_text;

} MainMenu;

#endif