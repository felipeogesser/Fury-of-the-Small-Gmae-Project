#ifndef STAT_SHEET_H
#define STAT_SHEET_H

#include <SDL2/SDL.h>

#define STAT_SHEET_W 340
#define STAT_SHEET_BARS 3
#define STAT_SHEET_ROWS 16
#define STAT_SHEET_CHIPS 12
#define STAT_SHEET_TEXT 48

typedef struct StatSheetBar {

    const char *label;
    char value[STAT_SHEET_TEXT];
    float ratio;
    SDL_Color fill;

} StatSheetBar;

typedef struct StatSheetRow {

    const char *label;
    char value[STAT_SHEET_TEXT];

} StatSheetRow;

typedef struct StatSheetChip {

    const char *label;
    SDL_Color color;

} StatSheetChip;

typedef struct StatSheet {

    const char *title;
    char subtitle[STAT_SHEET_TEXT];
    SDL_Color team;
    StatSheetBar bars[STAT_SHEET_BARS];
    unsigned int bar_count;
    StatSheetRow rows[STAT_SHEET_ROWS];
    unsigned int row_count;
    StatSheetChip chips[STAT_SHEET_CHIPS];
    unsigned int chip_count;
    const char *empty_chips;

} StatSheet;

__attribute__((format(printf, 3, 4)))
void stat_sheet_row(StatSheet *sheet, const char *label, const char *format, ...);

__attribute__((format(printf, 5, 6)))
void stat_sheet_bar(StatSheet *sheet, const char *label, float ratio, SDL_Color fill, const char *format, ...);

void stat_sheet_chip(StatSheet *sheet, const char *label, SDL_Color color);

SDL_Rect stat_sheet_rect(int x, int y, unsigned int bar_count, unsigned int row_count);
void stat_sheet_render(const StatSheet *sheet, int x, int y);

void stat_sheet_render_meter(SDL_Rect rect, float ratio, SDL_Color fill);
SDL_Color stat_sheet_health_color(float ratio);

#endif
