#include "loot_reveal.h"
#include "general_card.h"
#include "loot.h"
#include "spreadsheet.h"
#include "spreadsheet_internal.h"
#include "stat_sheet.h"
#include "ui.h"
#include "window_settings.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define REVEAL_W 900
#define REVEAL_H 640
#define REVEAL_SHEET_GAP 30
#define REVEAL_CARD_W 240
#define REVEAL_CARD_H 300
#define REVEAL_CARD_GAP 30
#define REVEAL_CARD_TOP 150
#define REVEAL_BUTTON_W 280
#define REVEAL_BUTTON_H 60
#define REVEAL_CHIP_H 26
#define REVEAL_CHIP_GAP 8
#define NO_CHOICE (-1)

typedef struct LootRevealState {

    UiButton take_button;
    int selected;
    int pressed;

} LootRevealState;

static LootRevealState state = {0};

static _Bool point_in(int x, int y, SDL_Rect rect) {

    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;

}

static SDL_Rect reveal_rect(void) {

    const int total = STAT_SHEET_W + REVEAL_SHEET_GAP + REVEAL_W;
    const int x = ((int)WINDOW_SIZE_X - total) / 2 + STAT_SHEET_W + REVEAL_SHEET_GAP;
    return (SDL_Rect){x, ((int)WINDOW_SIZE_Y - REVEAL_H) / 2, REVEAL_W, REVEAL_H};

}

static SDL_Rect card_rect(unsigned int index) {

    const SDL_Rect box = reveal_rect();
    const int total = (int)STAGE_ARMY_LOOT_CHOICES * REVEAL_CARD_W + ((int)STAGE_ARMY_LOOT_CHOICES - 1) * REVEAL_CARD_GAP;
    const int x = box.x + (box.w - total) / 2 + (int)index * (REVEAL_CARD_W + REVEAL_CARD_GAP);
    return (SDL_Rect){x, box.y + REVEAL_CARD_TOP, REVEAL_CARD_W, REVEAL_CARD_H};

}

static int choice_at(int x, int y) {

    for (unsigned int i = 0; i < STAGE_ARMY_LOOT_CHOICES; i++) {
        if (point_in(x, y, card_rect(i))) return (int)i;
    }
    return NO_CHOICE;

}

static int hovered_choice(void) {

    for (unsigned int i = 0; i < STAGE_ARMY_LOOT_CHOICES; i++) {
        if (ui_mouse_over(card_rect(i))) return (int)i;
    }
    return NO_CHOICE;

}

void loot_reveal_reset(void) {

    const SDL_Rect box = reveal_rect();
    state.take_button = ui_button(box.x + box.w / 2, box.y + box.h - 40 - REVEAL_BUTTON_H, REVEAL_BUTTON_W, REVEAL_BUTTON_H, "Take general", false);
    state.selected = NO_CHOICE;
    state.pressed = NO_CHOICE;

}

_Bool loot_reveal_is_open(void) {

    return loot_has_pending();

}

static void take_selected(void) {

    if (state.selected == NO_CHOICE) return;
    loot_take((unsigned int)state.selected);
    loot_reveal_reset();

}

static void select_choice(int index) {

    if (index < 0 || index >= (int)STAGE_ARMY_LOOT_CHOICES) return;
    state.selected = index;
    state.take_button.is_enabled = true;

}

static void key_input(SDL_Keycode key) {

    if (key >= SDLK_1 && key < SDLK_1 + (int)STAGE_ARMY_LOOT_CHOICES) select_choice((int)(key - SDLK_1));
    if (key >= SDLK_KP_1 && key < SDLK_KP_1 + (int)STAGE_ARMY_LOOT_CHOICES) select_choice((int)(key - SDLK_KP_1));
    if (key == SDLK_LEFT || key == SDLK_a) select_choice(state.selected == NO_CHOICE ? 0 : state.selected - 1);
    if (key == SDLK_RIGHT || key == SDLK_d) select_choice(state.selected == NO_CHOICE ? 0 : state.selected + 1);
    if (key == SDLK_RETURN || key == SDLK_KP_ENTER || key == SDLK_SPACE) take_selected();

}

_Bool loot_reveal_input(const SDL_Event *e) {

    if (!loot_reveal_is_open()) return false;

    if (e->type == SDL_KEYDOWN && !e->key.repeat) {
        key_input(e->key.keysym.sym);
        return true;
    }

    if (ui_button_event(&state.take_button, e)) {
        take_selected();
        return true;
    }

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) state.pressed = choice_at(e->button.x, e->button.y);

    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT) {
        const int released = choice_at(e->button.x, e->button.y);
        if (released != NO_CHOICE && released == state.pressed) {
            if (released == state.selected && e->button.clicks >= 2) take_selected();
            else select_choice(released);
        }
        state.pressed = NO_CHOICE;
    }

    return e->type == SDL_MOUSEBUTTONDOWN || e->type == SDL_MOUSEBUTTONUP || e->type == SDL_MOUSEWHEEL ||
           e->type == SDL_KEYDOWN || e->type == SDL_KEYUP;

}

static int chip_width(const char *label) {

    return ui_text_width(UI_FONT_SMALL, label) + 20;

}

static void render_traits(enum GeneralType type, int center_x, int y) {

    const BattalionSpreadsheet battalion = get_battalion_spreadsheet(spreadsheet_ptr->general_spreadsheet[type].battalion_type);

    const char *labels[5];
    unsigned int count = 0;
    if (battalion.is_shielded) labels[count++] = "Shielded";
    if (battalion.is_ranged) labels[count++] = "Ranged";
    if (battalion.is_large) labels[count++] = "Large";
    if (battalion.is_armor_piercing) labels[count++] = "Armor piercing";
    if (battalion.is_anti_large) labels[count++] = "Anti large";

    if (count == 0) {
        ui_text(UI_FONT_SMALL, "No special traits", center_x, y + 4, ui_color_disabled, UI_ALIGN_CENTER);
        return;
    }

    int total = 0;
    for (unsigned int i = 0; i < count; i++) total += chip_width(labels[i]) + (i > 0 ? REVEAL_CHIP_GAP : 0);

    int x = center_x - total / 2;
    for (unsigned int i = 0; i < count; i++) {
        const int w = chip_width(labels[i]);
        const SDL_Rect chip = {x, y, w, REVEAL_CHIP_H};
        ui_fill(chip, (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, 40});
        ui_frame(chip, (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, 200}, 1);
        ui_text(UI_FONT_SMALL, labels[i], x + w / 2, y + (REVEAL_CHIP_H - ui_font_height(UI_FONT_SMALL)) / 2, ui_color_accent, UI_ALIGN_CENTER);
        x += w + REVEAL_CHIP_GAP;
    }

}

static void render_card(unsigned int index, _Bool is_hovered) {

    const SDL_Rect rect = card_rect(index);
    const _Bool is_selected = (int)index == state.selected;

    if (is_selected) {
        const float pulse = 0.5f + 0.5f * sinf((float)SDL_GetTicks() * 0.005f);
        for (int i = 1; i <= 3; i++) {
            const int grow = i * 6;
            ui_frame((SDL_Rect){rect.x - grow, rect.y - grow, rect.w + 2 * grow, rect.h + 2 * grow},
                     (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, (Uint8)((30.0f + 60.0f * pulse) / (float)i)}, 2);
        }
    }

    general_card_render(loot_choice(index), rect, is_selected, is_hovered);

    char key[4];
    snprintf(key, sizeof(key), "%u", index + 1);
    const SDL_Rect badge = {rect.x + 8, rect.y + 8, 30, 30};
    ui_fill(badge, (SDL_Color){0, 0, 0, 160});
    ui_frame(badge, is_selected ? ui_color_accent : ui_color_panel_edge, 1);
    ui_text(UI_FONT_SMALL, key, badge.x + badge.w / 2, badge.y + (badge.h - ui_font_height(UI_FONT_SMALL)) / 2, is_selected ? ui_color_accent : ui_color_muted, UI_ALIGN_CENTER);

}

void loot_reveal_render(void) {

    if (!loot_reveal_is_open()) return;

    ui_fill((SDL_Rect){0, 0, (int)WINDOW_SIZE_X, (int)WINDOW_SIZE_Y}, (SDL_Color){0, 0, 0, 170});

    const SDL_Rect box = reveal_rect();
    const int center_x = box.x + box.w / 2;
    ui_fill((SDL_Rect){box.x + 6, box.y + 8, box.w, box.h}, (SDL_Color){0, 0, 0, 110});
    ui_panel(box);
    ui_fill((SDL_Rect){box.x, box.y, box.w, 5}, ui_color_accent);

    ui_text(UI_FONT_HEADING, "Spoils of victory", center_x, box.y + 30, ui_color_accent, UI_ALIGN_CENTER);
    ui_text(UI_FONT_BODY, "Choose a general to join your army", center_x, box.y + 92, ui_color_muted, UI_ALIGN_CENTER);

    const int hovered = hovered_choice();
    for (unsigned int i = 0; i < STAGE_ARMY_LOOT_CHOICES; i++) render_card(i, (int)i == hovered);

    const int focus = hovered != NO_CHOICE ? hovered : state.selected;
    const int traits_y = box.y + REVEAL_CARD_TOP + REVEAL_CARD_H + 26;
    if (focus != NO_CHOICE) {
        render_traits(loot_choice((unsigned int)focus), center_x, traits_y);
        const SDL_Rect sheet = general_card_sheet_rect(0, 0);
        general_card_render_sheet(loot_choice((unsigned int)focus), box.x - REVEAL_SHEET_GAP - STAT_SHEET_W, box.y + (box.h - sheet.h) / 2, "Reward");
    } else {
        ui_text(UI_FONT_SMALL, "Hover a general to see its stats", center_x, traits_y + 4, ui_color_disabled, UI_ALIGN_CENTER);
    }

    ui_button_render(&state.take_button);

}
