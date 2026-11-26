#include "battle_controls.h"
#include "engine_internal.h"
#include "ui.h"
#include "window_settings.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define CONTROLS_PLATE_W 250
#define CONTROLS_PLATE_H 70
#define CONTROLS_MARGIN 20

static const float speeds[] = {1.0f, 2.0f, 4.0f};

static _Bool is_paused = false;
static unsigned int speed_index = 0;

void battle_controls_reset(void) {

    is_paused = false;
    speed_index = 0;

}

_Bool battle_controls_input(const SDL_Event *e) {

    if (engine.has_program_been_initialized_with_ai_bypass) return false;
    if (e->type != SDL_KEYDOWN || e->key.repeat != 0) return false;

    switch (e->key.keysym.sym) {
        case SDLK_SPACE: is_paused = !is_paused; return true;
        case SDLK_1: speed_index = 0; return true;
        case SDLK_2: speed_index = 1; return true;
        case SDLK_3: speed_index = 2; return true;
        default: return false;
    }

}

_Bool battle_controls_is_paused(void) {

    return is_paused;

}

float battle_controls_time_scale(void) {

    if (engine.has_program_been_initialized_with_ai_bypass) return 1.0f;
    return is_paused ? 0.0f : speeds[speed_index];

}

SDL_Rect battle_controls_rect(void) {

    return (SDL_Rect){(int)WINDOW_SIZE_X - CONTROLS_MARGIN - CONTROLS_PLATE_W, 14, CONTROLS_PLATE_W, CONTROLS_PLATE_H};

}

void battle_controls_render(void) {

    if (engine.has_program_been_initialized_with_ai_bypass) return;

    const SDL_Rect plate = battle_controls_rect();
    ui_panel(plate);

    char state[32];
    SDL_Color color = ui_color_text;
    if (is_paused) {
        snprintf(state, sizeof(state), "Paused");
        const float pulse = 0.6f + 0.4f * sinf((float)SDL_GetTicks() * 0.006f);
        color = (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, (Uint8)(255.0f * pulse)};
    } else {
        snprintf(state, sizeof(state), "Speed %.0fx", (double)speeds[speed_index]);
    }

    ui_text(UI_FONT_BUTTON, state, plate.x + plate.w / 2, plate.y + 8, color, UI_ALIGN_CENTER);
    ui_text(UI_FONT_SMALL, "Space  1 2 3  H", plate.x + plate.w / 2, plate.y + plate.h - 8 - ui_font_height(UI_FONT_SMALL), ui_color_muted, UI_ALIGN_CENTER);

    if (is_paused) {
        const int w = ui_text_width(UI_FONT_HEADING, "Paused") + 80;
        const SDL_Rect banner = {((int)WINDOW_SIZE_X - w) / 2, 110, w, ui_font_height(UI_FONT_HEADING) + 24};
        ui_fill(banner, (SDL_Color){0, 0, 0, 130});
        ui_text(UI_FONT_HEADING, "Paused", banner.x + banner.w / 2, banner.y + 12, color, UI_ALIGN_CENTER);
    }

}
