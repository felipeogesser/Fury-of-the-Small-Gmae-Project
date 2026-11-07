#include "main_menu.h"
#include "main_menu_internal.h"
#include "engine_internal.h"
#include "run.h"
#include "scene_handler.h"
#include "scenes.h"
#include "ui.h"
#include "window_settings.h"
#include <stdbool.h>

#define MAIN_MENU_TITLE "C Game"
#define MAIN_MENU_BUTTON_WIDTH 380
#define MAIN_MENU_BUTTON_HEIGHT 66
#define MAIN_MENU_BUTTON_GAP 20
#define MAIN_MENU_BUTTONS_TOP 440

static MainMenu main_menu = {0};

void main_menu_init(void) {

    engine.main_menu = &main_menu;

    static const char *labels[MAIN_MENU_BUTTON_COUNT] = {
        [MAIN_MENU_NEW_RUN] = "New run",
        [MAIN_MENU_CONTINUE] = "Continue",
        [MAIN_MENU_SETTINGS] = "Settings",
        [MAIN_MENU_QUIT] = "Quit"
    };

    const int center_x = (int)WINDOW_SIZE_X / 2;
    for (unsigned int i = 0; i < MAIN_MENU_BUTTON_COUNT; i++) {

        const int y = MAIN_MENU_BUTTONS_TOP + (int)i * (MAIN_MENU_BUTTON_HEIGHT + MAIN_MENU_BUTTON_GAP);
        main_menu.buttons[i] = ui_button(center_x, y, MAIN_MENU_BUTTON_WIDTH, MAIN_MENU_BUTTON_HEIGHT, labels[i], true);

    }

    main_menu.buttons[MAIN_MENU_CONTINUE].is_enabled = run_is_active() && !run_is_complete();

}

void main_menu_input(SDL_Event *e) {

    if (ui_button_event(&main_menu.buttons[MAIN_MENU_NEW_RUN], e)) {
        run_new();
        scene_switch(STAGE_MAP);
        return;
    }

    if (ui_button_event(&main_menu.buttons[MAIN_MENU_CONTINUE], e)) {
        scene_switch(STAGE_MAP);
        return;
    }

    if (ui_button_event(&main_menu.buttons[MAIN_MENU_SETTINGS], e)) {
        scene_switch(SETTINGS);
        return;
    }

    if (ui_button_event(&main_menu.buttons[MAIN_MENU_QUIT], e)) {
        engine.window_running = false;
        return;
    }

}

void main_menu_render(void) {

    ui_backdrop();

    const int center_x = (int)WINDOW_SIZE_X / 2;
    ui_text(UI_FONT_TITLE, MAIN_MENU_TITLE, center_x + 4, 184, (SDL_Color){0, 0, 0, 140}, UI_ALIGN_CENTER);
    ui_text(UI_FONT_TITLE, MAIN_MENU_TITLE, center_x, 180, ui_color_accent, UI_ALIGN_CENTER);

    for (unsigned int i = 0; i < MAIN_MENU_BUTTON_COUNT; i++) ui_button_render(&main_menu.buttons[i]);

    ui_text(UI_FONT_SMALL, "Esc to quit", center_x, (int)WINDOW_SIZE_Y - 48, ui_color_muted, UI_ALIGN_CENTER);

}

void main_menu_destroy(void) {

}
