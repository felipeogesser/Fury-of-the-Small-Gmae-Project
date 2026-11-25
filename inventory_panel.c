#include "inventory_panel.h"
#include "deployment_loader.h"
#include "engine_internal.h"
#include "general_card.h"
#include "general_internal.h"
#include "general_sheet.h"
#include "inventory.h"
#include "stat_sheet.h"
#include "ui.h"
#include "window_settings.h"
#include <stdbool.h>
#include <stdio.h>

#define PANEL_W 840
#define PANEL_MARGIN 30
#define PANEL_PAD 30
#define PANEL_GRID_TOP 120
#define CARD_COLUMNS 5
#define CARD_GAP 15
#define CARD_H 176
#define CARD_STEP (CARD_H + CARD_GAP)
#define SHEET_GAP 20
#define SHEET_TOP 80
#define BUTTON_W 240
#define BUTTON_H 60
#define NO_CARD (-1)

typedef struct InventoryPanel {

    _Bool is_open;
    UiButton button;
    unsigned int order[INVENTORY_CAPACITY];
    unsigned int count;
    int selected;
    int pressed_card;
    _Bool is_outside_pressed;
    int scroll;

} InventoryPanel;

static InventoryPanel panel = {0};

static _Bool point_in(int x, int y, SDL_Rect rect) {

    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;

}

static SDL_Rect panel_rect(void) {

    return (SDL_Rect){(int)WINDOW_SIZE_X - PANEL_MARGIN - PANEL_W, PANEL_MARGIN, PANEL_W, (int)WINDOW_SIZE_Y - 2 * PANEL_MARGIN};

}

static SDL_Rect grid_rect(void) {

    const SDL_Rect outer = panel_rect();
    return (SDL_Rect){outer.x + PANEL_PAD, outer.y + PANEL_GRID_TOP, outer.w - 2 * PANEL_PAD, outer.h - PANEL_GRID_TOP - PANEL_PAD};

}

static SDL_Rect sheet_rect(void) {

    const SDL_Rect outer = panel_rect();
    return stat_sheet_rect(outer.x - SHEET_GAP - STAT_SHEET_W, SHEET_TOP, 0, GENERAL_SHEET_ROWS);

}

static int card_w(void) {

    return (grid_rect().w - (CARD_COLUMNS - 1) * CARD_GAP) / CARD_COLUMNS;

}

static int max_scroll(void) {

    const int rows = ((int)panel.count + CARD_COLUMNS - 1) / CARD_COLUMNS;
    const int content_h = rows * CARD_STEP - CARD_GAP;
    return content_h > grid_rect().h ? content_h - grid_rect().h : 0;

}

static SDL_Rect card_rect(unsigned int slot) {

    const SDL_Rect grid = grid_rect();
    const int column = (int)(slot % CARD_COLUMNS);
    const int row = (int)(slot / CARD_COLUMNS);
    return (SDL_Rect){grid.x + column * (card_w() + CARD_GAP), grid.y + row * CARD_STEP - panel.scroll, card_w(), CARD_H};

}

static int card_at(int x, int y) {

    if (!point_in(x, y, grid_rect())) return NO_CARD;
    for (unsigned int i = 0; i < panel.count; i++) {
        if (point_in(x, y, card_rect(i))) return (int)i;
    }
    return NO_CARD;

}

static void build_order(void) {

    panel.count = 0;
    for (int type = GENERAL_PADDING_FOR_ZERO + 1; type < GENERAL_UNIQUE; type++) {
        for (unsigned int i = 0; i < inventory_count(); i++) {
            if ((int)inventory_type_at(i) == type) panel.order[panel.count++] = i;
        }
    }

}

static void open_panel(void) {

    build_order();
    panel.is_open = true;
    panel.selected = NO_CARD;
    panel.pressed_card = NO_CARD;
    panel.is_outside_pressed = false;
    panel.scroll = 0;

}

static void close_panel(void) {

    panel.is_open = false;
    panel.selected = NO_CARD;
    panel.pressed_card = NO_CARD;
    panel.is_outside_pressed = false;

}

void inventory_panel_reset(void) {

    close_panel();
    panel.button = ui_button((int)WINDOW_SIZE_X - 40 - BUTTON_W / 2, (int)WINDOW_SIZE_Y - 110, BUTTON_W, BUTTON_H, "Inventory", true);

}

_Bool inventory_panel_is_open(void) {

    return panel.is_open;

}

SDL_Rect inventory_panel_button_rect(void) {

    return panel.button.rect;

}

static _Bool is_inside_window(int x, int y) {

    if (point_in(x, y, panel_rect())) return true;
    return panel.selected != NO_CARD && point_in(x, y, sheet_rect());

}

static void open_input(const SDL_Event *e) {

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
        close_panel();
        return;
    }

    if (e->type == SDL_MOUSEWHEEL) {
        panel.scroll -= e->wheel.y * CARD_STEP / 2;
        if (panel.scroll > max_scroll()) panel.scroll = max_scroll();
        if (panel.scroll < 0) panel.scroll = 0;
        return;
    }

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        panel.is_outside_pressed = !is_inside_window(e->button.x, e->button.y);
        panel.pressed_card = card_at(e->button.x, e->button.y);
        return;
    }

    if (e->type != SDL_MOUSEBUTTONUP || e->button.button != SDL_BUTTON_LEFT) return;

    if (panel.is_outside_pressed && !is_inside_window(e->button.x, e->button.y)) {
        close_panel();
        return;
    }

    const int released = card_at(e->button.x, e->button.y);
    if (released != NO_CARD && released == panel.pressed_card) panel.selected = released == panel.selected ? NO_CARD : released;
    panel.pressed_card = NO_CARD;
    panel.is_outside_pressed = false;

}

_Bool inventory_panel_input(const SDL_Event *e) {

    if (panel.is_open) {
        const _Bool is_mouse = e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP || e->type == SDL_MOUSEWHEEL || e->type == SDL_MOUSEMOTION;
        const _Bool is_key = e->type == SDL_KEYDOWN || e->type == SDL_KEYUP;
        open_input(e);
        return is_mouse || is_key;
    }

    if (ui_button_event(&panel.button, e)) {
        open_panel();
        return true;
    }
    return false;

}

static void render_sheet(void) {

    if (panel.selected == NO_CARD) return;

    const enum GeneralType type = inventory_type_at(panel.order[panel.selected]);
    General general = convert_spreadsheet_general_to_general(type);
    general.rarity = COMMON;

    StatSheet sheet = {0};
    general_sheet_fill(&sheet, &general);
    sheet.team = (SDL_Color){92, 150, 214, 255};
    snprintf(sheet.subtitle, sizeof(sheet.subtitle), "Your general  %s", general_sheet_rarity_name(general.rarity));

    const SDL_Rect rect = sheet_rect();
    stat_sheet_render(&sheet, rect.x, rect.y);

}

static void render_cards(void) {

    const SDL_Rect grid = grid_rect();
    const SDL_Rect clip = {grid.x - 6, grid.y, grid.w + 12, grid.h + 6};

    SDL_RenderSetClipRect(engine.renderer, &clip);
    for (unsigned int i = 0; i < panel.count; i++) {
        const SDL_Rect rect = card_rect(i);
        if (rect.y + rect.h < grid.y || rect.y > grid.y + grid.h) continue;
        const _Bool is_hovered = ui_mouse_over(rect) && ui_mouse_over(grid);
        general_card_render(inventory_type_at(panel.order[i]), rect, (int)i == panel.selected, is_hovered);
    }
    SDL_RenderSetClipRect(engine.renderer, NULL);

}

void inventory_panel_render(void) {

    if (!panel.is_open) {
        ui_button_render(&panel.button);
        return;
    }

    ui_fill((SDL_Rect){0, 0, (int)WINDOW_SIZE_X, (int)WINDOW_SIZE_Y}, (SDL_Color){0, 0, 0, 120});

    const SDL_Rect outer = panel_rect();
    ui_fill((SDL_Rect){outer.x + 6, outer.y + 8, outer.w, outer.h}, (SDL_Color){0, 0, 0, 110});
    ui_fill(outer, (SDL_Color){ui_color_panel.r, ui_color_panel.g, ui_color_panel.b, 248});
    ui_fill((SDL_Rect){outer.x, outer.y, outer.w, 5}, ui_color_accent);
    ui_frame(outer, ui_color_panel_edge, 2);

    ui_text(UI_FONT_HEADING, "Your army", outer.x + PANEL_PAD, outer.y + 28, ui_color_accent, UI_ALIGN_LEFT);

    char count[48];
    snprintf(count, sizeof(count), panel.count == 1 ? "%u general" : "%u generals", panel.count);
    ui_text(UI_FONT_BODY, count, outer.x + outer.w - PANEL_PAD, outer.y + 40, ui_color_muted, UI_ALIGN_RIGHT);
    ui_text(UI_FONT_SMALL, "Click a general to see its stats. Click outside to close.", outer.x + PANEL_PAD, outer.y + PANEL_GRID_TOP - 34, ui_color_muted, UI_ALIGN_LEFT);

    render_cards();
    render_sheet();

}
