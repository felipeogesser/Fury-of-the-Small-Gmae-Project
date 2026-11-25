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

#define REVEAL_W 620
#define REVEAL_H 600
#define REVEAL_ART 230
#define REVEAL_BUTTON_W 280
#define REVEAL_BUTTON_H 60
#define REVEAL_CHIP_H 26
#define REVEAL_CHIP_GAP 8

static UiButton accept_button = {0};

static SDL_Rect reveal_rect(void) {

    return (SDL_Rect){(int)WINDOW_SIZE_X / 2 - REVEAL_W / 2, (int)WINDOW_SIZE_Y / 2 - REVEAL_H / 2, REVEAL_W, REVEAL_H};

}

void loot_reveal_reset(void) {

    const SDL_Rect box = reveal_rect();
    accept_button = ui_button(box.x + box.w / 2, box.y + box.h - 40 - REVEAL_BUTTON_H, REVEAL_BUTTON_W, REVEAL_BUTTON_H, "Add to army", true);

}

_Bool loot_reveal_is_open(void) {

    enum GeneralType type = GENERAL_TYPE_UNKNOWN;
    return loot_peek_reveal(&type);

}

_Bool loot_reveal_input(const SDL_Event *e) {

    if (!loot_reveal_is_open()) return false;

    if (e->type == SDL_KEYDOWN && !e->key.repeat) {
        const SDL_Keycode key = e->key.keysym.sym;
        if (key == SDLK_RETURN || key == SDLK_KP_ENTER || key == SDLK_SPACE || key == SDLK_ESCAPE) {
            loot_dismiss_reveal();
            return true;
        }
    }

    if (ui_button_event(&accept_button, e)) loot_dismiss_reveal();

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

void loot_reveal_render(void) {

    enum GeneralType type = GENERAL_TYPE_UNKNOWN;
    if (!loot_peek_reveal(&type)) return;

    ui_fill((SDL_Rect){0, 0, (int)WINDOW_SIZE_X, (int)WINDOW_SIZE_Y}, (SDL_Color){0, 0, 0, 170});

    const SDL_Rect box = reveal_rect();
    const int center_x = box.x + box.w / 2;
    ui_fill((SDL_Rect){box.x + 6, box.y + 8, box.w, box.h}, (SDL_Color){0, 0, 0, 110});
    ui_panel(box);
    ui_fill((SDL_Rect){box.x, box.y, box.w, 5}, ui_color_accent);

    ui_text(UI_FONT_HEADING, "Spoils of victory", center_x, box.y + 30, ui_color_accent, UI_ALIGN_CENTER);
    ui_text(UI_FONT_BODY, "A new general joins your army", center_x, box.y + 92, ui_color_muted, UI_ALIGN_CENTER);

    const SDL_Rect art = {center_x - REVEAL_ART / 2, box.y + 140, REVEAL_ART, REVEAL_ART};
    const float pulse = 0.5f + 0.5f * sinf((float)SDL_GetTicks() * 0.004f);
    for (int i = 1; i <= 3; i++) {
        const int grow = i * 7;
        ui_frame((SDL_Rect){art.x - grow, art.y - grow, art.w + 2 * grow, art.h + 2 * grow},
                 (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, (Uint8)((30.0f + 50.0f * pulse) / (float)i)}, 2);
    }
    ui_fill(art, (SDL_Color){22, 23, 28, 255});
    general_card_render_sprite(type, art);
    ui_frame(art, ui_color_accent, 2);

    ui_text(UI_FONT_HEADING, general_card_name(type), center_x, art.y + art.h + 22, ui_color_text, UI_ALIGN_CENTER);
    render_traits(type, center_x, art.y + art.h + 22 + ui_font_height(UI_FONT_HEADING) + 10);

    ui_button_render(&accept_button);

}
