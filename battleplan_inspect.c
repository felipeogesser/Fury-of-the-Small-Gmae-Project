#include "battleplan_inspect.h"
#include "battleplan.h"
#include "general_internal.h"
#include "general_sheet.h"
#include "stat_sheet.h"
#include "ui.h"
#include "window_settings.h"
#include <stdbool.h>
#include <stdlib.h>

#define INSPECT_MARGIN 20
#define INSPECT_TOP 80
#define INSPECT_CLICK_SLOP 6
#define INSPECT_CLICK_MS 160

typedef struct Inspect {

    General *general;
    _Bool is_enemy;
    _Bool is_pressed;
    int press_x;
    int press_y;
    Uint32 press_ticks;

} Inspect;

static Inspect inspect = {0};

static int panel_x(void) {

    return inspect.is_enemy ? INSPECT_MARGIN : (int)WINDOW_SIZE_X - INSPECT_MARGIN - STAT_SHEET_W;

}

void battleplan_inspect_reset(void) {

    inspect = (Inspect){0};

}

_Bool battleplan_inspect_is_open(void) {

    return inspect.general != NULL;

}

SDL_Rect battleplan_inspect_rect(void) {

    return stat_sheet_rect(panel_x(), INSPECT_TOP, 0, GENERAL_SHEET_ROWS);

}

_Bool battleplan_inspect_input(const SDL_Event *e) {

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE && inspect.general != NULL) {
        inspect.general = NULL;
        return true;
    }

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        inspect.is_pressed = true;
        inspect.press_x = e->button.x;
        inspect.press_y = e->button.y;
        inspect.press_ticks = SDL_GetTicks();
        return false;
    }

    if (e->type != SDL_MOUSEBUTTONUP || e->button.button != SDL_BUTTON_LEFT || !inspect.is_pressed) return false;

    inspect.is_pressed = false;
    const _Bool is_click = abs(e->button.x - inspect.press_x) <= INSPECT_CLICK_SLOP &&
                           abs(e->button.y - inspect.press_y) <= INSPECT_CLICK_SLOP &&
                           SDL_GetTicks() - inspect.press_ticks <= INSPECT_CLICK_MS;
    if (!is_click) return false;

    _Bool is_enemy = false;
    General *general = battleplan_general_at(e->button.x, e->button.y, &is_enemy);
    if (general == NULL) {
        inspect.general = NULL;
        return false;
    }

    inspect.general = general == inspect.general ? NULL : general;
    inspect.is_enemy = is_enemy;
    return false;

}

void battleplan_inspect_render(void) {

    const General *general = inspect.general;
    if (general == NULL) return;

    StatSheet sheet = {0};
    general_sheet_fill(&sheet, general);
    sheet.team = inspect.is_enemy ? ui_color_danger : (SDL_Color){92, 150, 214, 255};
    if (inspect.is_enemy) snprintf(sheet.subtitle, sizeof(sheet.subtitle), "Enemy general");
    else snprintf(sheet.subtitle, sizeof(sheet.subtitle), "Your general  %s", general_sheet_rarity_name(general->rarity));

    stat_sheet_render(&sheet, panel_x(), INSPECT_TOP);

}
