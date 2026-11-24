#include "settings_menu.h"
#include "engine_internal.h"
#include "keybindings.h"
#include "scene_handler.h"
#include "scenes.h"
#include "ui.h"
#include "window_settings.h"
#include <stdbool.h>

#define SETTINGS_PANEL_WIDTH 1120
#define SETTINGS_ROW_HEIGHT 62
#define SETTINGS_PAD 60
#define SETTINGS_FULLSCREEN_Y 120
#define SETTINGS_CONTROLS_Y 230
#define SETTINGS_BINDINGS_Y 300

static int panel_height(void) {

    return SETTINGS_BINDINGS_Y + keybindings_height() + 40 + SETTINGS_ROW_HEIGHT + 40;

}

static UiButton fullscreen_button = {0};
static UiButton back_button = {0};

static _Bool is_fullscreen(void) {

    return (SDL_GetWindowFlags(engine.window) & SDL_WINDOW_FULLSCREEN_DESKTOP) != 0;

}

void settings_menu_init(void) {

    const int center_x = (int)WINDOW_SIZE_X / 2;
    const int panel_y = ((int)WINDOW_SIZE_Y - panel_height()) / 2;
    const int right_column = center_x + SETTINGS_PANEL_WIDTH / 2 - SETTINGS_PAD - 110;
    fullscreen_button = ui_button(right_column, panel_y + SETTINGS_FULLSCREEN_Y, 220, SETTINGS_ROW_HEIGHT, "", true);
    back_button = ui_button(center_x, panel_y + panel_height() - 40 - SETTINGS_ROW_HEIGHT, 300, SETTINGS_ROW_HEIGHT, "Back", true);

}

void settings_menu_input(SDL_Event *e) {

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
        scene_switch(MAIN_MENU);
        return;
    }

    if (ui_button_event(&fullscreen_button, e)) {
        SDL_SetWindowFullscreen(engine.window, is_fullscreen() ? 0 : SDL_WINDOW_FULLSCREEN_DESKTOP);
        return;
    }

    if (ui_button_event(&back_button, e)) {
        scene_switch(MAIN_MENU);
        return;
    }

}

void settings_menu_render(void) {

    ui_backdrop();

    const int center_x = (int)WINDOW_SIZE_X / 2;
    const int panel_y = ((int)WINDOW_SIZE_Y - panel_height()) / 2;
    const SDL_Rect panel = {center_x - SETTINGS_PANEL_WIDTH / 2, panel_y, SETTINGS_PANEL_WIDTH, panel_height()};
    ui_panel(panel);

    ui_text(UI_FONT_HEADING, "Settings", center_x, panel_y + 36, ui_color_accent, UI_ALIGN_CENTER);

    const int label_y = fullscreen_button.rect.y + (SETTINGS_ROW_HEIGHT - ui_font_height(UI_FONT_BODY)) / 2;
    ui_text(UI_FONT_BODY, "Fullscreen", panel.x + SETTINGS_PAD, label_y, ui_color_text, UI_ALIGN_LEFT);

    ui_text(UI_FONT_BODY, "Controls", panel.x + SETTINGS_PAD, panel_y + SETTINGS_CONTROLS_Y, ui_color_text, UI_ALIGN_LEFT);
    ui_text(UI_FONT_SMALL, "Fixed, cannot be changed", panel.x + panel.w - SETTINGS_PAD, panel_y + SETTINGS_CONTROLS_Y + 6, ui_color_muted, UI_ALIGN_RIGHT);
    ui_fill((SDL_Rect){panel.x + SETTINGS_PAD, panel_y + SETTINGS_CONTROLS_Y + 40, panel.w - 2 * SETTINGS_PAD, 1}, ui_color_panel_edge);
    keybindings_render((SDL_Rect){panel.x + SETTINGS_PAD, panel_y + SETTINGS_BINDINGS_Y, panel.w - 2 * SETTINGS_PAD, keybindings_height()});

    fullscreen_button.label = is_fullscreen() ? "On" : "Off";
    ui_button_render(&fullscreen_button);
    ui_button_render(&back_button);

}

void settings_menu_destroy(void) {

}
