#include "battlefield_hud.h"
#include "animation_types.h"
#include "battalion_types.h"
#include "battlefield_power_bar.h"
#include "engine_internal.h"
#include "fatigue_types.h"
#include "morale_types.h"
#include "spreadsheet.h"
#include "sprites_internal.h"
#include "ui.h"
#include "window_settings.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#define HUD_MARGIN 20
#define HUD_CARD_GAP 10
#define HUD_CARD_W 124
#define HUD_TAB_W 190
#define HUD_TAB_H 34
#define HUD_TAB_GAP 18
#define HUD_NO_CARD UINT32_MAX
#define HUD_CARD_H 168
#define HUD_PORTRAIT_H 80
#define HUD_BAR_H 14
#define HUD_DETAIL_W 540
#define HUD_DETAIL_PAD 22
#define HUD_DETAIL_GAP 14
#define HUD_ROW_H 28
#define HUD_STAT_ROWS 7

enum HudSide {
    HUD_SIDE_PLAYER,
    HUD_SIDE_ENEMY,
    HUD_SIDE_COUNT
};

typedef struct HudSelection {

    _Bool is_open;
    enum HudSide side;
    unsigned int index;

} HudSelection;

typedef struct HudStat {

    const char *label;
    char value[48];

} HudStat;

static const SDL_Rect portrait_crop = {32, 22, 38, 38};

static unsigned int hud_battalion_count = 0;
static enum HudSide shown_side = HUD_SIDE_PLAYER;
static HudSelection selection = {0};

static SDL_Rect card_rect(unsigned int index);
static SDL_Rect tab_rect(enum HudSide side);
static unsigned int card_at(int x, int y);
static unsigned int hovered_card(void);
static void show_side(enum HudSide side);
static void render_tab(const Table *tables, enum HudSide side);
static SDL_Rect detail_rect(SDL_Rect anchor);
static _Bool point_in(SDL_Rect rect, int x, int y);
static SDL_Color side_color(enum HudSide side);
static SDL_Color with_alpha(SDL_Color color, Uint8 alpha);
static SDL_Color health_color(float ratio);
static float clamp01(float value);
static const char *type_name(enum BattalionType type);
static const char *lifecycle_name(enum BattalionLifecycle state);
static const char *movement_name(enum BattalionMovementMode state);
static const char *morale_name(enum MoraleState state);
static const char *fatigue_name(enum FatigueState state);
static void render_bar(SDL_Rect rect, float ratio, SDL_Color fill);
static void render_portrait(SDL_Rect box, enum BattalionType type, enum HudSide side);
static int card_view_w(unsigned int index, _Bool is_on_top);
static void render_card(const Table *tables, enum HudSide side, unsigned int index, _Bool is_hovered, int view_w);
static void render_labeled_bar(int x, int y, int w, const char *label, const char *value, float ratio, SDL_Color fill);
static void render_stat_column(const HudStat *stats, unsigned int count, int x, int y, int w);
static int render_chip(int x, int y, int limit, const char *label, SDL_Color color);
static void render_detail(const Table *tables, enum HudSide side, unsigned int index);

void battlefield_hud_reset(unsigned int battalion_count) {

    hud_battalion_count = battalion_count;
    shown_side = HUD_SIDE_PLAYER;
    selection = (HudSelection){0};

}

_Bool battlefield_hud_input(const SDL_Event *e) {

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE && selection.is_open) {
        selection.is_open = false;
        return true;
    }

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_TAB) {
        show_side(shown_side == HUD_SIDE_PLAYER ? HUD_SIDE_ENEMY : HUD_SIDE_PLAYER);
        return true;
    }

    if (e->type != SDL_MOUSEBUTTONDOWN || e->button.button != SDL_BUTTON_LEFT) return false;

    const int x = e->button.x;
    const int y = e->button.y;

    if (selection.is_open && point_in(detail_rect(card_rect(selection.index)), x, y)) return true;

    for (unsigned int side = 0; side < HUD_SIDE_COUNT; side++) {
        if (point_in(tab_rect(side), x, y)) {
            show_side(side);
            return true;
        }
    }

    const unsigned int index = card_at(x, y);
    if (index != HUD_NO_CARD) {
        const _Bool is_same = selection.is_open && selection.index == index;
        selection = (HudSelection){!is_same, shown_side, index};
        return true;
    }

    selection.is_open = false;

    return false;

}

void battlefield_hud_render(const Table *player_tables, const Table *enemy_tables) {

    battlefield_power_bar_render(player_tables, enemy_tables, hud_battalion_count, side_color(HUD_SIDE_PLAYER), side_color(HUD_SIDE_ENEMY));

    render_tab(player_tables, HUD_SIDE_PLAYER);
    render_tab(enemy_tables, HUD_SIDE_ENEMY);

    const Table *tables = shown_side == HUD_SIDE_PLAYER ? player_tables : enemy_tables;
    const unsigned int selected = selection.is_open ? selection.index : HUD_NO_CARD;
    const unsigned int hovered = hovered_card();

    for (unsigned int i = 0; i < hud_battalion_count; i++) {
        if (i != selected && i != hovered) render_card(tables, shown_side, i, false, card_view_w(i, false));
    }
    if (hovered != HUD_NO_CARD && hovered != selected) render_card(tables, shown_side, hovered, true, card_view_w(hovered, true));
    if (selected < hud_battalion_count) {
        render_card(tables, shown_side, selected, selected == hovered, card_view_w(selected, true));
        render_detail(tables, shown_side, selected);
    }

}

static SDL_Rect card_rect(unsigned int index) {

    const int count = hud_battalion_count > 0 ? (int)hud_battalion_count : 1;
    const int available = (int)WINDOW_SIZE_X - 2 * HUD_MARGIN;
    int step = HUD_CARD_W + HUD_CARD_GAP;
    if (count > 1 && (count - 1) * step + HUD_CARD_W > available) step = (available - HUD_CARD_W) / (count - 1);

    const int row_w = (count - 1) * step + HUD_CARD_W;
    const int start_x = ((int)WINDOW_SIZE_X - row_w) / 2;
    const int y = (int)WINDOW_SIZE_Y - HUD_MARGIN - HUD_CARD_H;

    return (SDL_Rect){start_x + (int)index * step, y, HUD_CARD_W, HUD_CARD_H};

}

static int card_view_w(unsigned int index, _Bool is_on_top) {

    if (is_on_top || index + 1 >= hud_battalion_count) return HUD_CARD_W;
    const int step = card_rect(index + 1).x - card_rect(index).x;
    return step < HUD_CARD_W ? step : HUD_CARD_W;

}

static SDL_Rect tab_rect(enum HudSide side) {

    const int y = (int)WINDOW_SIZE_Y - HUD_MARGIN - HUD_CARD_H - HUD_TAB_GAP - HUD_TAB_H;
    const int x = (int)WINDOW_SIZE_X / 2 + (side == HUD_SIDE_PLAYER ? -HUD_TAB_W - 4 : 4);
    return (SDL_Rect){x, y, HUD_TAB_W, HUD_TAB_H};

}

static unsigned int card_at(int x, int y) {

    if (selection.is_open && selection.index < hud_battalion_count && point_in(card_rect(selection.index), x, y)) return selection.index;

    for (unsigned int i = hud_battalion_count; i-- > 0;) {
        if (point_in(card_rect(i), x, y)) return i;
    }

    return HUD_NO_CARD;

}

static unsigned int hovered_card(void) {

    if (selection.is_open && selection.index < hud_battalion_count && ui_mouse_over(card_rect(selection.index))) return selection.index;

    for (unsigned int i = hud_battalion_count; i-- > 0;) {
        if (ui_mouse_over(card_rect(i))) return i;
    }

    return HUD_NO_CARD;

}

static void show_side(enum HudSide side) {

    if (side == shown_side) return;
    shown_side = side;
    selection.is_open = false;

}

static void render_tab(const Table *tables, enum HudSide side) {

    const BattalionDynamicTable *bd = &tables->battalion_tables.friendly_dynamic;
    unsigned int present = 0;
    for (unsigned int i = 0; i < hud_battalion_count; i++) present += bd->is_present[i];

    const SDL_Rect tab = tab_rect(side);
    const _Bool is_active = side == shown_side;
    const _Bool is_hovered = ui_mouse_over(tab);
    const SDL_Color team = side_color(side);

    ui_fill((SDL_Rect){tab.x + 4, tab.y + 5, tab.w, tab.h}, (SDL_Color){0, 0, 0, 90});
    ui_fill(tab, is_active ? (SDL_Color){44, 48, 60, 245} : (SDL_Color){30, 32, 40, 220});
    if (is_active) ui_fill((SDL_Rect){tab.x, tab.y + tab.h - 4, tab.w, 4}, team);
    ui_frame(tab, is_active || is_hovered ? with_alpha(team, 200) : ui_color_panel_edge, 1);

    char label[48];
    snprintf(label, sizeof(label), "%s  %u/%u", side == HUD_SIDE_PLAYER ? "Left army" : "Right army", present, hud_battalion_count);
    const int text_y = tab.y + (tab.h - ui_font_height(UI_FONT_SMALL)) / 2 - 1;
    ui_text(UI_FONT_SMALL, label, tab.x + tab.w / 2, text_y, is_active ? team : ui_color_muted, UI_ALIGN_CENTER);

}

static SDL_Rect detail_rect(SDL_Rect anchor) {

    const int bars_h = 3 * (HUD_ROW_H + HUD_BAR_H + 8);
    const int h = HUD_DETAIL_PAD + 40 + 14 + bars_h + 10 + HUD_STAT_ROWS * HUD_ROW_H + 14 + 30 + HUD_DETAIL_PAD;

    int x = anchor.x + anchor.w / 2 - HUD_DETAIL_W / 2;
    if (x < HUD_MARGIN) x = HUD_MARGIN;
    if (x + HUD_DETAIL_W > (int)WINDOW_SIZE_X - HUD_MARGIN) x = (int)WINDOW_SIZE_X - HUD_MARGIN - HUD_DETAIL_W;

    return (SDL_Rect){x, tab_rect(HUD_SIDE_PLAYER).y - HUD_DETAIL_GAP - h, HUD_DETAIL_W, h};

}

static _Bool point_in(SDL_Rect rect, int x, int y) {

    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;

}

static SDL_Color side_color(enum HudSide side) {

    return side == HUD_SIDE_PLAYER ? (SDL_Color){92, 150, 214, 255} : ui_color_danger;

}

static SDL_Color with_alpha(SDL_Color color, Uint8 alpha) {

    return (SDL_Color){color.r, color.g, color.b, alpha};

}

static SDL_Color health_color(float ratio) {

    if (ratio > 0.5f) return ui_color_success;
    if (ratio > 0.25f) return ui_color_accent;
    return ui_color_danger;

}

static float clamp01(float value) {

    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;

}

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

static const char *lifecycle_name(enum BattalionLifecycle state) {

    switch (state) {
        case NORMAL: return "Steady";
        case ROUTING: return "Routing";
        case WITHDRAWN: return "Withdrawn";
        case WIPED_OUT: return "Wiped out";
        default: return "-";
    }

}

static const char *movement_name(enum BattalionMovementMode state) {

    switch (state) {
        case STATIONARY: return "Holding";
        case WALKING: return "Walking";
        case RUNNING: return "Running";
        case CHARGING: return "Charging";
        case FIGHTING: return "Fighting";
        default: return "-";
    }

}

static const char *morale_name(enum MoraleState state) {

    switch (state) {
        case EAGER: return "Eager";
        case CONFIDENT: return "Confident";
        case WAVERING: return "Wavering";
        case SHATTERED: return "Shattered";
        default: return "-";
    }

}

static const char *fatigue_name(enum FatigueState state) {

    switch (state) {
        case FRESH: return "Fresh";
        case ACTIVE: return "Active";
        case WINDED: return "Winded";
        case TIRED: return "Tired";
        case VERY_TIRED: return "Very tired";
        case EXHAUSTED: return "Exhausted";
        default: return "-";
    }

}

static void render_bar(SDL_Rect rect, float ratio, SDL_Color fill) {

    ui_fill(rect, (SDL_Color){12, 13, 16, 255});
    const int fill_w = (int)lroundf((float)(rect.w - 4) * clamp01(ratio));
    if (fill_w > 0) {
        ui_fill((SDL_Rect){rect.x + 2, rect.y + 2, fill_w, rect.h - 4}, fill);
        ui_fill((SDL_Rect){rect.x + 2, rect.y + 2, fill_w, (rect.h - 4) / 3}, (SDL_Color){255, 255, 255, 40});
    }
    ui_frame(rect, ui_color_panel_edge, 1);

}

static void render_portrait(SDL_Rect box, enum BattalionType type, enum HudSide side) {

    ui_fill(box, (SDL_Color){20, 22, 28, 255});
    ui_fill((SDL_Rect){box.x, box.y + box.h - box.h / 3, box.w, box.h / 3}, with_alpha(side_color(side), 30));

    const enum Sprites sprite_type = get_battalion_spreadsheet(type).sprite_type;
    if (sprite_type < 0 || sprite_type >= SPRITES_COUNT) return;
    const Sprite *sprite = &engine.sprite_pack->sprite[sprite_type][IDLE];
    if (sprite->texture == NULL || sprite->frames_count == 0) return;

    const int size = box.h < box.w ? box.h : box.w;
    const SDL_Rect target = {box.x + (box.w - size) / 2, box.y, size, size};
    const SDL_RendererFlip flip = side == HUD_SIDE_ENEMY ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE;
    SDL_RenderCopyEx(engine.renderer, sprite->texture, &portrait_crop, &target, 0.0, NULL, flip);

}

static void render_card(const Table *tables, enum HudSide side, unsigned int index, _Bool is_hovered, int view_w) {

    const BattalionStaticTable *bs = &tables->battalion_tables.friendly_static;
    const BattalionDynamicTable *bd = &tables->battalion_tables.friendly_dynamic;
    const SDL_Rect card = card_rect(index);
    const _Bool is_selected = selection.is_open && selection.side == side && selection.index == index;
    const _Bool is_present = bd->is_present[index];
    const enum BattalionLifecycle lifecycle = bd->lifecycle_state[index];
    const SDL_Color team = side_color(side);
    const int lift = is_selected ? 8 : (is_hovered ? 3 : 0);
    const SDL_Rect body = {card.x, card.y - lift, card.w, card.h};

    ui_fill((SDL_Rect){card.x + 4, card.y + 6, card.w, card.h}, (SDL_Color){0, 0, 0, 100});
    ui_fill(body, ui_color_panel);
    ui_fill((SDL_Rect){body.x, body.y, body.w, 4}, team);

    SDL_Rect previous_clip;
    SDL_RenderGetClipRect(engine.renderer, &previous_clip);
    const _Bool had_clip = SDL_RenderIsClipEnabled(engine.renderer);
    const SDL_Rect view = {body.x, body.y, view_w, body.h};
    const SDL_Rect clip = {view.x + 3, view.y, view.w - 6, view.h};
    SDL_RenderSetClipRect(engine.renderer, &clip);

    const SDL_Rect portrait = {view.x + 8, view.y + 12, view.w - 16, HUD_PORTRAIT_H};
    render_portrait(portrait, bs->battalion_type[index], side);

    const int name_y = portrait.y + portrait.h + 6;
    const char *name = type_name(bs->battalion_type[index]);
    const int name_w = ui_text_width(UI_FONT_SMALL, name);
    const int name_x = name_w <= view.w - 12 ? view.x + (view.w - name_w) / 2 : view.x + 6;
    ui_text(UI_FONT_SMALL, name, name_x, name_y, ui_color_text, UI_ALIGN_LEFT);

    const float max_hp = bs->max_hp[index];
    const float hp = bd->hp[index] > 0.0f ? bd->hp[index] : 0.0f;
    const float ratio = max_hp > 0.0f ? hp / max_hp : 0.0f;
    const SDL_Rect bar = {view.x + 8, name_y + ui_font_height(UI_FONT_SMALL) + 4, view.w - 16, HUD_BAR_H};
    render_bar(bar, ratio, health_color(ratio));

    char hp_text[32];
    snprintf(hp_text, sizeof(hp_text), "%d%%", (int)lroundf(ratio * 100.0f));
    ui_text(UI_FONT_SMALL, hp_text, view.x + view.w / 2, bar.y + bar.h + 2, ui_color_muted, UI_ALIGN_CENTER);

    SDL_RenderSetClipRect(engine.renderer, had_clip ? &previous_clip : NULL);

    if (!is_present) {
        ui_fill(body, (SDL_Color){10, 10, 14, 170});
        ui_text(UI_FONT_SMALL, lifecycle_name(lifecycle), view.x + view.w / 2, portrait.y + portrait.h / 2 - 10, ui_color_danger, UI_ALIGN_CENTER);
    } else if (lifecycle == ROUTING) {
        const SDL_Rect tag = {portrait.x, portrait.y + portrait.h - 22, portrait.w, 22};
        ui_fill(tag, (SDL_Color){0, 0, 0, 170});
        ui_text(UI_FONT_SMALL, "Routing", view.x + view.w / 2, tag.y + 1, ui_color_accent, UI_ALIGN_CENTER);
    }

    if (is_selected) ui_frame(body, ui_color_accent, 3);
    else ui_frame(body, is_hovered ? with_alpha(ui_color_accent, 160) : ui_color_panel_edge, 2);

}

static void render_labeled_bar(int x, int y, int w, const char *label, const char *value, float ratio, SDL_Color fill) {

    ui_text(UI_FONT_SMALL, label, x, y, ui_color_muted, UI_ALIGN_LEFT);
    ui_text(UI_FONT_SMALL, value, x + w, y, ui_color_text, UI_ALIGN_RIGHT);
    render_bar((SDL_Rect){x, y + HUD_ROW_H - 4, w, HUD_BAR_H}, ratio, fill);

}

static void render_stat_column(const HudStat *stats, unsigned int count, int x, int y, int w) {

    for (unsigned int i = 0; i < count; i++) {

        const int row_y = y + (int)i * HUD_ROW_H;
        if (i % 2 == 0) ui_fill((SDL_Rect){x - 6, row_y - 2, w + 12, HUD_ROW_H}, (SDL_Color){255, 255, 255, 8});
        ui_text(UI_FONT_SMALL, stats[i].label, x, row_y, ui_color_muted, UI_ALIGN_LEFT);
        ui_text(UI_FONT_SMALL, stats[i].value, x + w, row_y, ui_color_text, UI_ALIGN_RIGHT);

    }

}

static int render_chip(int x, int y, int limit, const char *label, SDL_Color color) {

    const int w = ui_text_width(UI_FONT_SMALL, label) + 16;
    if (x + w > limit) return x;
    const SDL_Rect chip = {x, y, w, 26};
    ui_fill(chip, with_alpha(color, 40));
    ui_frame(chip, with_alpha(color, 200), 1);
    ui_text(UI_FONT_SMALL, label, x + 8, y + 2, color, UI_ALIGN_LEFT);
    return x + w + 8;

}

static void render_detail(const Table *tables, enum HudSide side, unsigned int index) {

    const BattalionStaticTable *bs = &tables->battalion_tables.friendly_static;
    const BattalionDynamicTable *bd = &tables->battalion_tables.friendly_dynamic;
    const GeneralDynamicTable *gd = &tables->general_tables.friendly_dynamic;
    const enum BattalionType type = bs->battalion_type[index];
    const BattalionSpreadsheet sheet = get_battalion_spreadsheet(type);
    const SDL_Color team = side_color(side);
    const SDL_Rect anchor = card_rect(index);
    const SDL_Rect panel = detail_rect(anchor);

    ui_fill((SDL_Rect){panel.x + 6, panel.y + 8, panel.w, panel.h}, (SDL_Color){0, 0, 0, 110});
    ui_fill(panel, (SDL_Color){ui_color_panel.r, ui_color_panel.g, ui_color_panel.b, 245});
    ui_fill((SDL_Rect){panel.x, panel.y, panel.w, 5}, team);
    ui_frame(panel, ui_color_panel_edge, 2);

    const int x = panel.x + HUD_DETAIL_PAD;
    const int w = panel.w - 2 * HUD_DETAIL_PAD;
    int y = panel.y + HUD_DETAIL_PAD;

    ui_text(UI_FONT_BUTTON, type_name(type), x, y, ui_color_text, UI_ALIGN_LEFT);
    char subtitle[48];
    snprintf(subtitle, sizeof(subtitle), "%s  #%u", side == HUD_SIDE_PLAYER ? "Left army" : "Right army", index + 1);
    ui_text(UI_FONT_SMALL, subtitle, x + w, y + 6, team, UI_ALIGN_RIGHT);
    y += 40;
    ui_fill((SDL_Rect){x, y, w, 1}, ui_color_panel_edge);
    y += 14;

    const float max_hp = bs->max_hp[index];
    const float hp = bd->hp[index] > 0.0f ? bd->hp[index] : 0.0f;
    const float hp_ratio = max_hp > 0.0f ? hp / max_hp : 0.0f;
    const float morale_ratio = sheet.morale > 0.0f ? bd->morale[index] / sheet.morale : 0.0f;
    const float stamina_ratio = sheet.fatigue > 0.0f ? bd->fatigue[index] / sheet.fatigue : 0.0f;
    const int bar_step = HUD_ROW_H + HUD_BAR_H + 8;

    char value[48];
    snprintf(value, sizeof(value), "%d / %d", (int)lroundf(hp), (int)lroundf(max_hp));
    render_labeled_bar(x, y, w, "Health", value, hp_ratio, health_color(hp_ratio));
    y += bar_step;

    snprintf(value, sizeof(value), "%s  %d", morale_name(bd->morale_state[index]), (int)lroundf(bd->morale[index]));
    render_labeled_bar(x, y, w, "Morale", value, morale_ratio, ui_color_accent);
    y += bar_step;

    snprintf(value, sizeof(value), "%s  %d%%", fatigue_name(bd->fatigue_state[index]), (int)lroundf(clamp01(stamina_ratio) * 100.0f));
    render_labeled_bar(x, y, w, "Stamina", value, stamina_ratio, (SDL_Color){110, 160, 200, 255});
    y += bar_step + 10;

    HudStat left[HUD_STAT_ROWS] = {
        {"Men", ""}, {"Fallen", ""}, {"Withdrawn", ""}, {"Melee attack", ""},
        {"Melee defense", ""}, {"Strength", ""}, {"Armor", ""}
    };
    snprintf(left[0].value, sizeof(left[0].value), "%u / %u", bd->alive_entity_count[index], bs->start_entity_count[index]);
    snprintf(left[1].value, sizeof(left[1].value), "%u", bd->dead_entity_count[index]);
    snprintf(left[2].value, sizeof(left[2].value), "%u", bd->withdrawed_entity_count[index]);
    snprintf(left[3].value, sizeof(left[3].value), "%.0f", (double)bd->meele_attack[index]);
    snprintf(left[4].value, sizeof(left[4].value), "%.0f", (double)bd->meele_defense[index]);
    snprintf(left[5].value, sizeof(left[5].value), "%.0f", (double)bd->strength[index]);
    snprintf(left[6].value, sizeof(left[6].value), "%.0f", (double)bs->armor[index]);

    HudStat right[HUD_STAT_ROWS] = {
        {"Status", ""}, {"Movement", ""}, {"Speed", ""}, {"Range", ""},
        {"Charge bonus", ""}, {"Attack speed", ""}, {"General", ""}
    };
    snprintf(right[0].value, sizeof(right[0].value), "%s", lifecycle_name(bd->lifecycle_state[index]));
    snprintf(right[1].value, sizeof(right[1].value), "%s", movement_name(bd->movement_state[index]));
    snprintf(right[2].value, sizeof(right[2].value), "%.0f", (double)bd->speed[index]);
    snprintf(right[3].value, sizeof(right[3].value), "%.0f", (double)bs->range[index]);
    snprintf(right[4].value, sizeof(right[4].value), "%.0f", (double)bs->charge_bonus[index]);
    snprintf(right[5].value, sizeof(right[5].value), "%.2f", (double)bs->attack_speed[index]);
    if (gd->is_dead[index]) snprintf(right[6].value, sizeof(right[6].value), "Fallen");
    else if (!gd->is_present[index]) snprintf(right[6].value, sizeof(right[6].value), "Withdrawn");
    else snprintf(right[6].value, sizeof(right[6].value), "%.0f hp", (double)gd->hp[index]);

    const int column_gap = 36;
    const int column_w = (w - column_gap) / 2;
    render_stat_column(left, HUD_STAT_ROWS, x, y, column_w);
    render_stat_column(right, HUD_STAT_ROWS, x + column_w + column_gap, y, column_w);
    y += HUD_STAT_ROWS * HUD_ROW_H + 14;

    int chip_x = x;
    if (bs->is_shielded[index]) chip_x = render_chip(chip_x, y, x + w, "Shielded", ui_color_text);
    if (bs->is_ranged[index]) chip_x = render_chip(chip_x, y, x + w, "Ranged", ui_color_text);
    if (bs->is_large[index]) chip_x = render_chip(chip_x, y, x + w, "Large", ui_color_text);
    if (bs->is_armor_piercing[index]) chip_x = render_chip(chip_x, y, x + w, "Armor piercing", ui_color_text);
    if (bs->is_anti_large[index]) chip_x = render_chip(chip_x, y, x + w, "Anti large", ui_color_text);
    if (bd->is_charging[index]) chip_x = render_chip(chip_x, y, x + w, "Charging", ui_color_accent);
    if (bd->is_engaged[index]) chip_x = render_chip(chip_x, y, x + w, "Engaged", ui_color_danger);
    if (bd->is_bracing[index]) chip_x = render_chip(chip_x, y, x + w, "Bracing", ui_color_accent);
    if (bd->is_headless[index]) chip_x = render_chip(chip_x, y, x + w, "Leaderless", ui_color_danger);
    if (chip_x == x) ui_text(UI_FONT_SMALL, "No special traits", x, y + 2, ui_color_disabled, UI_ALIGN_LEFT);

}
