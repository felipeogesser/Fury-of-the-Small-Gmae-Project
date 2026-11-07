#ifndef MAIN_MENU_INTERNAL_H
#define MAIN_MENU_INTERNAL_H

#include "main_menu_types.h"
#include "ui.h"

enum MainMenuButton {
    MAIN_MENU_NEW_RUN,
    MAIN_MENU_CONTINUE,
    MAIN_MENU_SETTINGS,
    MAIN_MENU_QUIT,
    MAIN_MENU_BUTTON_COUNT
};

typedef struct MainMenu {

    UiButton buttons[MAIN_MENU_BUTTON_COUNT];

} MainMenu;

#endif
