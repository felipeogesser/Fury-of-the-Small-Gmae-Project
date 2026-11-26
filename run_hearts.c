#include "run_hearts.h"
#include "run.h"
#include "run_stats.h"
#include "ui.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

#define HEART_SIZE 46
#define HEART_GAP 12
#define HEART_BORDER 3
#define HEART_LOST_MS 900u
#define HEART_GROW 0.5f

static const SDL_Color heart_fill = {214, 58, 70, 255};
static const SDL_Color heart_edge = {96, 22, 30, 255};
static const SDL_Color heart_empty = {40, 34, 40, 255};
static const SDL_Color heart_empty_edge = {80, 70, 78, 255};

static Uint32 lost_at = 0;
static _Bool is_losing = false;

static _Bool is_inside(float x, float y) {

    const float a = x * x + y * y - 1.0f;
    return a * a * a - x * x * y * y * y <= 0.0f;

}

static void fill_heart(int center_x, int center_y, float size, SDL_Color color) {

    if (size < 2.0f) return;

    const float scale = size / 2.4f;
    const int top = center_y - (int)(1.2f * scale);
    const int rows = (int)(2.3f * scale) + 1;
    const int half = (int)(1.2f * scale) + 1;

    for (int r = 0; r < rows; r++) {
        const float y = 1.25f - ((float)r + 0.5f) / scale;
        int span_start = 0;
        _Bool is_in_span = false;
        for (int c = -half; c <= half + 1; c++) {
            const _Bool is_in = c <= half && is_inside(((float)c + 0.5f) / scale, y);
            if (is_in && !is_in_span) span_start = c;
            if (!is_in && is_in_span) ui_fill((SDL_Rect){center_x + span_start, top + r, c - span_start, 1}, color);
            is_in_span = is_in;
        }
    }

}

static void render_heart(int center_x, int center_y, float size, _Bool is_full, Uint8 alpha) {

    const SDL_Color edge = is_full ? heart_edge : heart_empty_edge;
    const SDL_Color fill = is_full ? heart_fill : heart_empty;
    fill_heart(center_x, center_y, size, (SDL_Color){edge.r, edge.g, edge.b, alpha});
    fill_heart(center_x, center_y, size - 2.0f * HEART_BORDER * 1.4f, (SDL_Color){fill.r, fill.g, fill.b, alpha});

    if (!is_full) return;
    const int shine = (int)(size * 0.14f);
    ui_fill((SDL_Rect){center_x - (int)(size * 0.3f), center_y - (int)(size * 0.22f), shine, shine}, (SDL_Color){255, 255, 255, (Uint8)(alpha * 90 / 255)});

}

void run_hearts_reset(void) {

    is_losing = run_take_lost();
    lost_at = SDL_GetTicks();

}

void run_hearts_render(int x, int y) {

    const unsigned int lives = run_stats_lives();
    const Uint32 elapsed = SDL_GetTicks() - lost_at;
    const _Bool is_animating = is_losing && elapsed < HEART_LOST_MS && lives < RUN_LIVES;

    for (unsigned int i = 0; i < RUN_LIVES; i++) {
        const int center_x = x + HEART_SIZE / 2 + (int)i * (HEART_SIZE + HEART_GAP);
        const int center_y = y + HEART_SIZE / 2;
        render_heart(center_x, center_y, (float)HEART_SIZE, i < lives, 255);

        if (is_animating && i == lives) {
            const float t = (float)elapsed / (float)HEART_LOST_MS;
            render_heart(center_x, center_y, (float)HEART_SIZE * (1.0f + HEART_GROW * t), true, (Uint8)(255.0f * (1.0f - t)));
        }
    }

}
