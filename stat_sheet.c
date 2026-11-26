#include "stat_sheet.h"
#include "ui.h"
#include <math.h>
#include <stdarg.h>
#include <stdio.h>

#define SHEET_PAD 18
#define SHEET_HEADER_H 70
#define SHEET_BAR_STEP 42
#define SHEET_BAR_H 14
#define SHEET_ROW_H 22
#define SHEET_CHIP_H 24
#define SHEET_CHIP_GAP 5
#define SHEET_CHIP_ROWS 4

typedef struct ChipFlow {

    int left;
    int right;
    int x;
    int y;

} ChipFlow;

static float clamp01(float value) {

    if (value < 0.0f) return 0.0f;
    if (value > 1.0f) return 1.0f;
    return value;

}

static SDL_Color with_alpha(SDL_Color color, Uint8 alpha) {

    return (SDL_Color){color.r, color.g, color.b, alpha};

}

void stat_sheet_row(StatSheet *sheet, const char *label, const char *format, ...) {

    if (sheet->row_count >= STAT_SHEET_ROWS) return;
    StatSheetRow *row = &sheet->rows[sheet->row_count++];
    row->label = label;
    va_list args;
    va_start(args, format);
    vsnprintf(row->value, sizeof(row->value), format, args);
    va_end(args);

}

void stat_sheet_bar(StatSheet *sheet, const char *label, float ratio, SDL_Color fill, const char *format, ...) {

    if (sheet->bar_count >= STAT_SHEET_BARS) return;
    StatSheetBar *bar = &sheet->bars[sheet->bar_count++];
    bar->label = label;
    bar->ratio = ratio;
    bar->fill = fill;
    va_list args;
    va_start(args, format);
    vsnprintf(bar->value, sizeof(bar->value), format, args);
    va_end(args);

}

void stat_sheet_chip(StatSheet *sheet, const char *label, SDL_Color color) {

    if (sheet->chip_count >= STAT_SHEET_CHIPS) return;
    sheet->chips[sheet->chip_count++] = (StatSheetChip){label, color};

}

SDL_Rect stat_sheet_rect(int x, int y, unsigned int bar_count, unsigned int row_count) {

    const int bars_h = bar_count > 0 ? (int)bar_count * SHEET_BAR_STEP + 8 : 0;
    const int h = SHEET_PAD + SHEET_HEADER_H + bars_h + (int)row_count * SHEET_ROW_H + 12 + SHEET_CHIP_ROWS * (SHEET_CHIP_H + SHEET_CHIP_GAP) + SHEET_PAD;
    return (SDL_Rect){x, y, STAT_SHEET_W, h};

}

void stat_sheet_render_meter(SDL_Rect rect, float ratio, SDL_Color fill) {

    ui_fill(rect, (SDL_Color){12, 13, 16, 255});
    const int fill_w = (int)lroundf((float)(rect.w - 4) * clamp01(ratio));
    if (fill_w > 0) {
        ui_fill((SDL_Rect){rect.x + 2, rect.y + 2, fill_w, rect.h - 4}, fill);
        ui_fill((SDL_Rect){rect.x + 2, rect.y + 2, fill_w, (rect.h - 4) / 3}, (SDL_Color){255, 255, 255, 40});
    }
    ui_frame(rect, ui_color_panel_edge, 1);

}

SDL_Color stat_sheet_health_color(float ratio) {

    if (ratio > 0.5f) return ui_color_success;
    if (ratio > 0.25f) return ui_color_accent;
    return ui_color_danger;

}

static void render_chip(ChipFlow *flow, const StatSheetChip *chip) {

    const int w = ui_text_width(UI_FONT_SMALL, chip->label) + 16;
    if (flow->x > flow->left && flow->x + w > flow->right) {
        flow->x = flow->left;
        flow->y += SHEET_CHIP_H + SHEET_CHIP_GAP;
    }

    const SDL_Rect rect = {flow->x, flow->y, w, SHEET_CHIP_H};
    ui_fill(rect, with_alpha(chip->color, 40));
    ui_frame(rect, with_alpha(chip->color, 200), 1);
    ui_text(UI_FONT_SMALL, chip->label, flow->x + 8, flow->y + (SHEET_CHIP_H - ui_font_height(UI_FONT_SMALL)) / 2, chip->color, UI_ALIGN_LEFT);
    flow->x += w + SHEET_CHIP_GAP;

}

void stat_sheet_render(const StatSheet *sheet, int panel_x, int panel_y) {

    const SDL_Rect panel = stat_sheet_rect(panel_x, panel_y, sheet->bar_count, sheet->row_count);

    ui_fill((SDL_Rect){panel.x + 6, panel.y + 8, panel.w, panel.h}, (SDL_Color){0, 0, 0, 110});
    ui_fill(panel, (SDL_Color){ui_color_panel.r, ui_color_panel.g, ui_color_panel.b, 245});
    ui_fill((SDL_Rect){panel.x, panel.y, panel.w, 5}, sheet->team);
    ui_frame(panel, ui_color_panel_edge, 2);

    const int x = panel.x + SHEET_PAD;
    const int w = panel.w - 2 * SHEET_PAD;
    int y = panel.y + SHEET_PAD;

    ui_text(UI_FONT_BUTTON, sheet->title, x, y, ui_color_text, UI_ALIGN_LEFT);
    ui_text(UI_FONT_SMALL, sheet->subtitle, x, y + ui_font_height(UI_FONT_BUTTON) + 2, sheet->team, UI_ALIGN_LEFT);
    ui_fill((SDL_Rect){x, y + SHEET_HEADER_H - 10, w, 1}, ui_color_panel_edge);
    y += SHEET_HEADER_H;

    for (unsigned int i = 0; i < sheet->bar_count; i++) {
        const StatSheetBar *bar = &sheet->bars[i];
        ui_text(UI_FONT_SMALL, bar->label, x, y, ui_color_muted, UI_ALIGN_LEFT);
        ui_text(UI_FONT_SMALL, bar->value, x + w, y, ui_color_text, UI_ALIGN_RIGHT);
        stat_sheet_render_meter((SDL_Rect){x, y + SHEET_ROW_H, w, SHEET_BAR_H}, bar->ratio, bar->fill);
        y += SHEET_BAR_STEP;
    }
    if (sheet->bar_count > 0) y += 8;

    for (unsigned int i = 0; i < sheet->row_count; i++) {
        const StatSheetRow *row = &sheet->rows[i];
        if (i % 2 == 0) ui_fill((SDL_Rect){x - 6, y, w + 12, SHEET_ROW_H}, (SDL_Color){255, 255, 255, 8});
        ui_text(UI_FONT_SMALL, row->label, x, y, ui_color_muted, UI_ALIGN_LEFT);
        ui_text(UI_FONT_SMALL, row->value, x + w, y, ui_color_text, UI_ALIGN_RIGHT);
        y += SHEET_ROW_H;
    }
    y += 12;

    ChipFlow flow = {x, x + w, x, y};
    for (unsigned int i = 0; i < sheet->chip_count; i++) render_chip(&flow, &sheet->chips[i]);
    if (sheet->chip_count == 0) ui_text(UI_FONT_SMALL, sheet->empty_chips ? sheet->empty_chips : "No special traits", x, y + 2, ui_color_disabled, UI_ALIGN_LEFT);

}
