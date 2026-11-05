#include "stage_map.h"
#include "run.h"
#include "scene_handler.h"
#include "scenes.h"
#include "ui.h"
#include "window_settings.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>

#define STAGE_SPACING 300.0f
#define STAGE_BOX 140
#define STAGE_BOX_CURRENT 180
#define STAGE_TRACK_Y 500
#define STAGE_SCROLL_DELAY 0.5f
#define STAGE_SCROLL_RATE 4.0f

static float scroll = 0.0f;
static float scroll_delay = 0.0f;
static _Bool is_current_pressed = false;
static UiButton back_button = {0};

static float stage_target(void) {

    const unsigned int stage = run_is_complete() ? RUN_STAGE_COUNT - 1 : run_current_stage();
    return (float)stage * STAGE_SPACING;

}

static int stage_screen_x(unsigned int stage) {

    return (int)lroundf((float)WINDOW_SIZE_X / 2.0f + (float)stage * STAGE_SPACING - scroll);

}

static SDL_Rect stage_rect(unsigned int stage) {

    const _Bool is_current = !run_is_complete() && stage == run_current_stage();
    const int size = is_current ? STAGE_BOX_CURRENT : STAGE_BOX;
    return (SDL_Rect){stage_screen_x(stage) - size / 2, STAGE_TRACK_Y - size / 2, size, size};

}

static _Bool is_settled(void) {

    return scroll_delay <= 0.0f && fabsf(stage_target() - scroll) < 1.0f;

}

static _Bool point_in(int x, int y, SDL_Rect rect) {

    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;

}

void stage_map_init(void) {

    if (!run_is_active()) run_new();

    if (run_take_advanced()) {
        scroll = stage_target() - STAGE_SPACING;
        scroll_delay = STAGE_SCROLL_DELAY;
    } else {
        scroll = stage_target();
        scroll_delay = 0.0f;
    }

    is_current_pressed = false;
    back_button = ui_button(170, (int)WINDOW_SIZE_Y - 110, 240, 60, "Back", true);

}

void stage_map_input(SDL_Event *e) {

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
        scene_switch(MAIN_MENU);
        return;
    }

    if (ui_button_event(&back_button, e)) {
        scene_switch(MAIN_MENU);
        return;
    }

    if (run_is_complete() || !is_settled()) {
        is_current_pressed = false;
        return;
    }

    const SDL_Rect current = stage_rect(run_current_stage());

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        is_current_pressed = point_in(e->button.x, e->button.y, current);
        return;
    }

    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT) {
        const _Bool was_pressed = is_current_pressed;
        is_current_pressed = false;
        if (was_pressed && point_in(e->button.x, e->button.y, current)) scene_switch(BATTLEPLAN);
        return;
    }

}

void stage_map_update(float dt) {

    if (scroll_delay > 0.0f) {
        scroll_delay -= dt;
        return;
    }

    const float target = stage_target();
    const float step = 1.0f - expf(-STAGE_SCROLL_RATE * dt);
    scroll += (target - scroll) * step;
    if (fabsf(target - scroll) < 0.5f) scroll = target;

}

static void render_track(void) {

    const int first = stage_screen_x(0);
    const int last = stage_screen_x(RUN_STAGE_COUNT - 1);
    const unsigned int reached = run_is_complete() ? RUN_STAGE_COUNT - 1 : run_current_stage();
    const int reached_x = stage_screen_x(reached);

    ui_fill((SDL_Rect){first, STAGE_TRACK_Y - 3, last - first, 6}, (SDL_Color){58, 60, 70, 255});
    if (reached_x > first) ui_fill((SDL_Rect){first, STAGE_TRACK_Y - 3, reached_x - first, 6}, ui_color_accent);

}

static void render_stage(unsigned int stage) {

    const SDL_Rect rect = stage_rect(stage);
    if (rect.x + rect.w < 0 || rect.x > (int)WINDOW_SIZE_X) return;

    const _Bool is_complete = run_is_complete() || stage < run_current_stage();
    const _Bool is_current = !run_is_complete() && stage == run_current_stage();
    const _Bool is_hovered = is_current && is_settled() && ui_mouse_over(rect);

    char number[8];
    snprintf(number, sizeof(number), "%u", stage + 1);
    const int number_y = rect.y + (rect.h - ui_font_height(UI_FONT_HEADING)) / 2;
    const int caption_y = rect.y + rect.h + 18;

    ui_fill((SDL_Rect){rect.x + 6, rect.y + 8, rect.w, rect.h}, (SDL_Color){0, 0, 0, 110});

    if (is_current) {

        const float pulse = 0.5f + 0.5f * sinf((float)SDL_GetTicks() * 0.004f);
        const Uint8 glow = (Uint8)(40.0f + 60.0f * pulse);
        for (int i = 1; i <= 3; i++) {
            const int grow = i * 8;
            ui_frame((SDL_Rect){rect.x - grow, rect.y - grow, rect.w + 2 * grow, rect.h + 2 * grow},
                     (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, (Uint8)(glow / i)}, 2);
        }

        const int sink = (is_current_pressed && is_hovered) ? 3 : 0;
        const SDL_Rect body = {rect.x, rect.y + sink, rect.w, rect.h};
        ui_fill(body, is_hovered ? (SDL_Color){104, 82, 40, 255} : (SDL_Color){78, 62, 34, 255});
        ui_frame(body, ui_color_accent, 3);
        ui_text(UI_FONT_HEADING, number, body.x + body.w / 2, number_y + sink, ui_color_text, UI_ALIGN_CENTER);
        ui_text(UI_FONT_BODY, "Fight", body.x + body.w / 2, caption_y, ui_color_accent, UI_ALIGN_CENTER);
        return;

    }

    if (is_complete) {

        ui_fill(rect, (SDL_Color){30, 36, 32, 255});
        ui_frame(rect, (SDL_Color){70, 96, 70, 255}, 2);
        ui_text(UI_FONT_HEADING, number, rect.x + rect.w / 2, number_y, (SDL_Color){96, 112, 98, 255}, UI_ALIGN_CENTER);
        ui_text(UI_FONT_SMALL, "Won", rect.x + rect.w / 2, caption_y, (SDL_Color){96, 132, 96, 255}, UI_ALIGN_CENTER);
        return;

    }

    ui_fill(rect, (SDL_Color){30, 32, 40, 255});
    ui_frame(rect, ui_color_panel_edge, 2);
    ui_text(UI_FONT_HEADING, number, rect.x + rect.w / 2, number_y, ui_color_muted, UI_ALIGN_CENTER);
    if (stage == RUN_STAGE_COUNT - 1) ui_text(UI_FONT_SMALL, "Final", rect.x + rect.w / 2, caption_y, ui_color_muted, UI_ALIGN_CENTER);

}

void stage_map_render(void) {

    ui_backdrop();

    const int center_x = (int)WINDOW_SIZE_X / 2;
    ui_fill((SDL_Rect){0, STAGE_TRACK_Y - 170, (int)WINDOW_SIZE_X, 340}, (SDL_Color){14, 15, 20, 170});

    char heading[32];
    if (run_is_complete()) snprintf(heading, sizeof(heading), "Run complete");
    else snprintf(heading, sizeof(heading), "Stage %u of %u", run_current_stage() + 1, RUN_STAGE_COUNT);
    ui_text(UI_FONT_HEADING, heading, center_x, 150, ui_color_accent, UI_ALIGN_CENTER);

    const char *hint = run_is_complete() ? "Every stage beaten" : "Click the highlighted stage to plan the battle";
    ui_text(UI_FONT_BODY, hint, center_x, 220, ui_color_muted, UI_ALIGN_CENTER);

    render_track();
    for (unsigned int stage = 0; stage < RUN_STAGE_COUNT; stage++) render_stage(stage);

    ui_button_render(&back_button);

}

void stage_map_destroy(void) {

    is_current_pressed = false;

}
