#include "battleplan_inspect.h"
#include "battleplan.h"
#include "general_internal.h"
#include "spreadsheet.h"
#include "stat_sheet.h"
#include "ui.h"
#include "window_settings.h"
#include <stdbool.h>
#include <stdlib.h>

#define INSPECT_MARGIN 20
#define INSPECT_TOP 80
#define INSPECT_CLICK_SLOP 6
#define INSPECT_CLICK_MS 160
#define INSPECT_ROWS 13

typedef struct Inspect {

    General *general;
    _Bool is_enemy;
    _Bool is_pressed;
    int press_x;
    int press_y;
    Uint32 press_ticks;

} Inspect;

static Inspect inspect = {0};

static const char *type_name(enum BattalionType type) {

    switch (type) {
        case TEMPLAR: return "Templar";
        case CAVALRY: return "Cavalry";
        case GUARDSMEN: return "Guardsmen";
        case SWORDSMEN: return "Swordsmen";
        case ARCHERS: return "Archers";
        default: return "Battalion";
    }

}

static const char *rarity_name(enum Rarity rarity) {

    switch (rarity) {
        case UNIQUE: return "Unique";
        case RARE: return "Rare";
        case COMMON: return "Common";
        default: return "";
    }

}

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

    return stat_sheet_rect(panel_x(), INSPECT_TOP, 0, INSPECT_ROWS);

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

    const BattalionSpreadsheet battalion = get_battalion_spreadsheet(general->battalion_type);

    StatSheet sheet = {0};
    sheet.title = type_name(general->battalion_type);
    sheet.team = inspect.is_enemy ? ui_color_danger : (SDL_Color){92, 150, 214, 255};
    if (inspect.is_enemy) snprintf(sheet.subtitle, sizeof(sheet.subtitle), "Enemy general");
    else snprintf(sheet.subtitle, sizeof(sheet.subtitle), "Your general  %s", rarity_name(general->rarity));

    stat_sheet_row(&sheet, "Health per man", "%.0f", (double)battalion.hp);
    stat_sheet_row(&sheet, "Morale", "%.0f", (double)battalion.morale);
    stat_sheet_row(&sheet, "Stamina", "%.0f", (double)battalion.fatigue);
    stat_sheet_row(&sheet, "Speed", "%.0f", (double)battalion.speed);
    stat_sheet_row(&sheet, "Melee attack", "%.0f", (double)battalion.meele_attack);
    stat_sheet_row(&sheet, "Melee defense", "%.0f", (double)battalion.meele_defense);
    stat_sheet_row(&sheet, "Strength", "%.0f", (double)battalion.strength);
    stat_sheet_row(&sheet, "Armor", "%.0f", (double)battalion.armor);
    stat_sheet_row(&sheet, "Range", "%.0f", (double)battalion.range);
    stat_sheet_row(&sheet, "Charge bonus", "%.0f", (double)battalion.charge_bonus);
    stat_sheet_row(&sheet, "Attack speed", "%.2f", (double)battalion.attack_speed);
    stat_sheet_row(&sheet, "General health", "%d", general->hp);
    stat_sheet_row(&sheet, "General attack", "%d", general->base_meele_attack);

    if (battalion.is_shielded) stat_sheet_chip(&sheet, "Shielded", ui_color_text);
    if (battalion.is_ranged) stat_sheet_chip(&sheet, "Ranged", ui_color_text);
    if (battalion.is_large) stat_sheet_chip(&sheet, "Large", ui_color_text);
    if (battalion.is_armor_piercing) stat_sheet_chip(&sheet, "Armor piercing", ui_color_text);
    if (battalion.is_anti_large) stat_sheet_chip(&sheet, "Anti large", ui_color_text);

    stat_sheet_render(&sheet, panel_x(), INSPECT_TOP);

}
