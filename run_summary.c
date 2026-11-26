#include "run_summary.h"
#include "general_card.h"
#include "inventory.h"
#include "run.h"
#include "run_stats.h"
#include "scene_handler.h"
#include "scenes.h"
#include "ui.h"
#include "window_settings.h"
#include <stdbool.h>
#include <stdio.h>

#define SUMMARY_W 1300
#define SUMMARY_H 900
#define SUMMARY_PAD 60
#define STAT_COUNT 5
#define STAT_W 210
#define STAT_H 110
#define STAT_GAP 20
#define PICK_W 104
#define PICK_GAP 12
#define ARMY_ENTRY_W 220
#define ARMY_ENTRY_GAP 10
#define ARMY_ART 90
#define BUTTON_W 240
#define BUTTON_H 60
#define BUTTON_GAP 30

enum SummaryButton {
    SUMMARY_CLOSE,
    SUMMARY_MAIN_MENU,
    SUMMARY_NEW_RUN,
    SUMMARY_BUTTON_COUNT
};

typedef struct RunSummary {

    _Bool is_open;
    UiButton buttons[SUMMARY_BUTTON_COUNT];

} RunSummary;

static RunSummary summary = {0};

static SDL_Rect summary_rect(void) {

    return (SDL_Rect){((int)WINDOW_SIZE_X - SUMMARY_W) / 2, ((int)WINDOW_SIZE_Y - SUMMARY_H) / 2, SUMMARY_W, SUMMARY_H};

}

void run_summary_reset(void) {

    summary.is_open = run_is_active() && run_is_complete();

    const SDL_Rect box = summary_rect();
    const int y = box.y + box.h - 40 - BUTTON_H;
    const int step = BUTTON_W + BUTTON_GAP;
    const int center_x = box.x + box.w / 2;
    summary.buttons[SUMMARY_CLOSE] = ui_button(center_x - step, y, BUTTON_W, BUTTON_H, "Close", true);
    summary.buttons[SUMMARY_MAIN_MENU] = ui_button(center_x, y, BUTTON_W, BUTTON_H, "Main menu", true);
    summary.buttons[SUMMARY_NEW_RUN] = ui_button(center_x + step, y, BUTTON_W, BUTTON_H, "New run", true);

}

_Bool run_summary_is_open(void) {

    return summary.is_open;

}

_Bool run_summary_input(const SDL_Event *e) {

    if (!summary.is_open) return false;

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
        summary.is_open = false;
        return true;
    }

    if (ui_button_event(&summary.buttons[SUMMARY_CLOSE], e)) {
        summary.is_open = false;
        return true;
    }

    if (ui_button_event(&summary.buttons[SUMMARY_MAIN_MENU], e)) {
        summary.is_open = false;
        scene_switch(MAIN_MENU);
        return true;
    }

    if (ui_button_event(&summary.buttons[SUMMARY_NEW_RUN], e)) {
        summary.is_open = false;
        run_new();
        scene_switch(STAGE_MAP);
        return true;
    }

    return e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP || e->type == SDL_MOUSEWHEEL ||
           e->type == SDL_KEYDOWN || e->type == SDL_KEYUP;

}

static unsigned int picks_earned(void) {

    unsigned int count = 0;
    for (unsigned int i = 0; i < RUN_STAGE_COUNT; i++) count += run_stats_pick(i) != GENERAL_PADDING_FOR_ZERO;
    return count;

}

static void render_stat(SDL_Rect rect, const char *label, const char *value) {

    ui_fill(rect, (SDL_Color){22, 23, 28, 255});
    ui_frame(rect, ui_color_panel_edge, 1);
    ui_text(UI_FONT_HEADING, value, rect.x + rect.w / 2, rect.y + 18, ui_color_text, UI_ALIGN_CENTER);
    ui_text(UI_FONT_SMALL, label, rect.x + rect.w / 2, rect.y + rect.h - 18 - ui_font_height(UI_FONT_SMALL), ui_color_muted, UI_ALIGN_CENTER);

}

static int render_stats(int center_x, int y) {

    char values[STAT_COUNT][16];
    const char *labels[STAT_COUNT] = {"Stages won", "Battles fought", "Defeats", "Generals earned", "Army size"};
    const unsigned int won = RUN_STAGE_COUNT;
    snprintf(values[0], sizeof(values[0]), "%u / %u", won, RUN_STAGE_COUNT);
    snprintf(values[1], sizeof(values[1]), "%u", won + run_stats_losses());
    snprintf(values[2], sizeof(values[2]), "%u", run_stats_losses());
    snprintf(values[3], sizeof(values[3]), "%u", picks_earned());
    snprintf(values[4], sizeof(values[4]), "%u", inventory_count());

    const int total = STAT_COUNT * STAT_W + (STAT_COUNT - 1) * STAT_GAP;
    for (int i = 0; i < STAT_COUNT; i++) {
        const SDL_Rect rect = {center_x - total / 2 + i * (STAT_W + STAT_GAP), y, STAT_W, STAT_H};
        render_stat(rect, labels[i], values[i]);
    }
    return y + STAT_H;

}

static int render_picks(int center_x, int y) {

    const int total = (int)RUN_STAGE_COUNT * PICK_W + ((int)RUN_STAGE_COUNT - 1) * PICK_GAP;
    for (unsigned int i = 0; i < RUN_STAGE_COUNT; i++) {
        const SDL_Rect rect = {center_x - total / 2 + (int)i * (PICK_W + PICK_GAP), y, PICK_W, PICK_W};
        const enum GeneralType type = run_stats_pick(i);
        const _Bool has_pick = type != GENERAL_PADDING_FOR_ZERO;

        ui_fill(rect, (SDL_Color){22, 23, 28, 255});
        if (has_pick) general_card_render_sprite(type, (SDL_Rect){rect.x + 4, rect.y + 4, rect.w - 8, rect.h - 8});
        else ui_text(UI_FONT_BODY, "-", rect.x + rect.w / 2, rect.y + (rect.h - ui_font_height(UI_FONT_BODY)) / 2, ui_color_disabled, UI_ALIGN_CENTER);
        ui_frame(rect, ui_color_panel_edge, 1);

        char number[8];
        snprintf(number, sizeof(number), "%u", i + 1);
        const SDL_Rect badge = {rect.x + 4, rect.y + 4, 28, 24};
        ui_fill(badge, (SDL_Color){0, 0, 0, 160});
        ui_text(UI_FONT_SMALL, number, badge.x + badge.w / 2, badge.y + (badge.h - ui_font_height(UI_FONT_SMALL)) / 2, ui_color_muted, UI_ALIGN_CENTER);

        if (has_pick) ui_text(UI_FONT_SMALL, general_card_name(type), rect.x + rect.w / 2, rect.y + rect.h + 8, ui_color_text, UI_ALIGN_CENTER);
    }
    return y + PICK_W + 8 + ui_font_height(UI_FONT_SMALL);

}

static int render_army(int center_x, int y) {

    const int types = GENERAL_UNIQUE - GENERAL_PADDING_FOR_ZERO - 1;
    const int total = types * ARMY_ENTRY_W + (types - 1) * ARMY_ENTRY_GAP;

    for (int t = 0; t < types; t++) {
        const enum GeneralType type = (enum GeneralType)(GENERAL_PADDING_FOR_ZERO + 1 + t);
        unsigned int count = 0;
        for (unsigned int i = 0; i < inventory_count(); i++) count += inventory_type_at(i) == type;

        const int x = center_x - total / 2 + t * (ARMY_ENTRY_W + ARMY_ENTRY_GAP);
        const SDL_Rect entry = {x, y, ARMY_ENTRY_W, ARMY_ART + 16};
        ui_fill(entry, (SDL_Color){22, 23, 28, 255});
        ui_frame(entry, ui_color_panel_edge, 1);

        const SDL_Rect art = {x + 8, y + 8, ARMY_ART, ARMY_ART};
        general_card_render_sprite(type, art);

        char amount[16];
        snprintf(amount, sizeof(amount), "x%u", count);
        const int text_x = art.x + art.w + 12;
        ui_text(UI_FONT_SMALL, general_card_name(type), text_x, y + 22, ui_color_muted, UI_ALIGN_LEFT);
        ui_text(UI_FONT_HEADING, amount, text_x, y + 22 + ui_font_height(UI_FONT_SMALL) + 4, count > 0 ? ui_color_text : ui_color_disabled, UI_ALIGN_LEFT);
    }
    return y + ARMY_ART + 16;

}

void run_summary_render(void) {

    if (!summary.is_open) return;

    ui_fill((SDL_Rect){0, 0, (int)WINDOW_SIZE_X, (int)WINDOW_SIZE_Y}, (SDL_Color){0, 0, 0, 180});

    const SDL_Rect box = summary_rect();
    const int center_x = box.x + box.w / 2;
    ui_fill((SDL_Rect){box.x + 6, box.y + 8, box.w, box.h}, (SDL_Color){0, 0, 0, 110});
    ui_panel(box);
    ui_fill((SDL_Rect){box.x, box.y, box.w, 5}, ui_color_accent);

    int y = box.y + 36;
    ui_text(UI_FONT_TITLE, "Victory", center_x, y, ui_color_accent, UI_ALIGN_CENTER);
    y += ui_font_height(UI_FONT_TITLE) + 6;

    char subtitle[48];
    snprintf(subtitle, sizeof(subtitle), "All %u stages conquered", RUN_STAGE_COUNT);
    ui_text(UI_FONT_BODY, subtitle, center_x, y, ui_color_muted, UI_ALIGN_CENTER);
    y += ui_font_height(UI_FONT_BODY) + 34;

    y = render_stats(center_x, y) + 36;

    ui_text(UI_FONT_BODY, "Spoils of the campaign", box.x + SUMMARY_PAD, y, ui_color_accent, UI_ALIGN_LEFT);
    y += ui_font_height(UI_FONT_BODY) + 14;
    y = render_picks(center_x, y) + 30;

    ui_text(UI_FONT_BODY, "Final army", box.x + SUMMARY_PAD, y, ui_color_accent, UI_ALIGN_LEFT);
    y += ui_font_height(UI_FONT_BODY) + 14;
    render_army(center_x, y);

    for (unsigned int i = 0; i < SUMMARY_BUTTON_COUNT; i++) ui_button_render(&summary.buttons[i]);

}
