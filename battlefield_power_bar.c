#include "battlefield_power_bar.h"
#include "army_health.h"
#include "ui.h"
#include "window_settings.h"
#include <math.h>
#include <stdio.h>

#define POWER_PLATE_W 760
#define POWER_PLATE_H 70
#define POWER_PLATE_Y 14
#define POWER_PAD 16
#define POWER_BAR_H 22

void battlefield_power_bar_render(const Table *left_tables, const Table *right_tables, unsigned int battalion_count, unsigned int battalion_size, SDL_Color left_color, SDL_Color right_color) {

    const float left_hp = army_health_on_field(left_tables, battalion_count, battalion_size);
    const float right_hp = army_health_on_field(right_tables, battalion_count, battalion_size);
    const float total = left_hp + right_hp;
    const float left_share = total > 0.0f ? left_hp / total : 0.5f;

    const SDL_Rect plate = {(int)WINDOW_SIZE_X / 2 - POWER_PLATE_W / 2, POWER_PLATE_Y, POWER_PLATE_W, POWER_PLATE_H};
    ui_panel(plate);

    const int text_y = plate.y + 8;
    ui_text(UI_FONT_SMALL, "Left army", plate.x + POWER_PAD, text_y, left_color, UI_ALIGN_LEFT);
    ui_text(UI_FONT_SMALL, "Balance of power", plate.x + plate.w / 2, text_y, ui_color_muted, UI_ALIGN_CENTER);
    ui_text(UI_FONT_SMALL, "Right army", plate.x + plate.w - POWER_PAD, text_y, right_color, UI_ALIGN_RIGHT);

    const SDL_Rect bar = {plate.x + POWER_PAD, plate.y + plate.h - POWER_PAD - POWER_BAR_H, plate.w - 2 * POWER_PAD, POWER_BAR_H};
    const SDL_Rect inner = {bar.x + 2, bar.y + 2, bar.w - 4, bar.h - 4};
    const int split = inner.x + (int)lroundf((float)inner.w * left_share);

    ui_fill(bar, (SDL_Color){12, 13, 16, 255});
    ui_fill((SDL_Rect){inner.x, inner.y, split - inner.x, inner.h}, left_color);
    ui_fill((SDL_Rect){split, inner.y, inner.x + inner.w - split, inner.h}, right_color);
    ui_fill((SDL_Rect){inner.x, inner.y, inner.w, inner.h / 3}, (SDL_Color){255, 255, 255, 40});
    ui_fill((SDL_Rect){inner.x + inner.w / 2 - 1, bar.y - 4, 2, bar.h + 8}, (SDL_Color){ui_color_text.r, ui_color_text.g, ui_color_text.b, 120});
    ui_fill((SDL_Rect){split - 1, inner.y, 3, inner.h}, ui_color_text);
    ui_frame(bar, ui_color_panel_edge, 1);

    char left_text[16];
    char right_text[16];
    const int left_pct = (int)lroundf(left_share * 100.0f);
    snprintf(left_text, sizeof(left_text), "%d%%", left_pct);
    snprintf(right_text, sizeof(right_text), "%d%%", 100 - left_pct);
    const int pct_y = bar.y + (bar.h - ui_font_height(UI_FONT_SMALL)) / 2;
    ui_text(UI_FONT_SMALL, left_text, inner.x + 6, pct_y, ui_color_text, UI_ALIGN_LEFT);
    ui_text(UI_FONT_SMALL, right_text, inner.x + inner.w - 6, pct_y, ui_color_text, UI_ALIGN_RIGHT);

}
