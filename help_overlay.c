#include "help_overlay.h"
#include "keybindings.h"
#include "ui.h"
#include "window_settings.h"
#include <stdbool.h>

#define HELP_PANEL_W 1080
#define HELP_PAD 48

static _Bool is_open = false;

void help_overlay_close(void) {

    is_open = false;

}

_Bool help_overlay_is_open(void) {

    return is_open;

}

_Bool help_overlay_input(const SDL_Event *e) {

    if (e->type == SDL_KEYDOWN && e->key.repeat == 0 && (e->key.keysym.sym == SDLK_h || e->key.keysym.sym == SDLK_F1)) {
        is_open = !is_open;
        return true;
    }

    if (!is_open) return false;

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
        is_open = false;
        return true;
    }

    if (e->type == SDL_MOUSEBUTTONDOWN) {
        is_open = false;
        return true;
    }

    return e->type == SDL_KEYDOWN || e->type == SDL_MOUSEBUTTONUP || e->type == SDL_MOUSEWHEEL;

}

void help_overlay_render(void) {

    if (!is_open) return;

    ui_fill((SDL_Rect){0, 0, (int)WINDOW_SIZE_X, (int)WINDOW_SIZE_Y}, (SDL_Color){0, 0, 0, 150});

    const int title_h = ui_font_height(UI_FONT_HEADING);
    const int h = HELP_PAD + title_h + 30 + keybindings_height() + 30 + ui_font_height(UI_FONT_SMALL) + HELP_PAD;
    const SDL_Rect panel = {((int)WINDOW_SIZE_X - HELP_PANEL_W) / 2, ((int)WINDOW_SIZE_Y - h) / 2, HELP_PANEL_W, h};

    ui_fill((SDL_Rect){panel.x + 8, panel.y + 10, panel.w, panel.h}, (SDL_Color){0, 0, 0, 120});
    ui_panel(panel);
    ui_fill((SDL_Rect){panel.x, panel.y, panel.w, 5}, ui_color_accent);

    const int center_x = panel.x + panel.w / 2;
    ui_text(UI_FONT_HEADING, "Controls", center_x, panel.y + HELP_PAD, ui_color_accent, UI_ALIGN_CENTER);

    const SDL_Rect area = {panel.x + HELP_PAD, panel.y + HELP_PAD + title_h + 30, panel.w - 2 * HELP_PAD, keybindings_height()};
    keybindings_render(area);

    ui_text(UI_FONT_SMALL, "Press H, Esc or click anywhere to close", center_x, panel.y + panel.h - HELP_PAD - ui_font_height(UI_FONT_SMALL), ui_color_muted, UI_ALIGN_CENTER);

}
