#include "load_menu.h"
#include "engine_internal.h"
#include "run.h"
#include "save.h"
#include "save_slots.h"
#include "scene_handler.h"
#include "scenes.h"
#include "ui.h"
#include "window_settings.h"
#include <stdbool.h>
#include <stdio.h>
#include <time.h>

#define LOAD_PANEL_W 1100
#define LOAD_PANEL_Y 70
#define LOAD_PANEL_H 860
#define LOAD_PAD 50
#define LOAD_LIST_TOP 130
#define LOAD_ROW_H 76
#define LOAD_ROW_GAP 10
#define LOAD_ROW_STEP (LOAD_ROW_H + LOAD_ROW_GAP)
#define LOAD_VISIBLE_ROWS 7
#define LOAD_LIST_H (LOAD_VISIBLE_ROWS * LOAD_ROW_STEP - LOAD_ROW_GAP)
#define LOAD_ROW_BUTTON_W 130
#define LOAD_ROW_BUTTON_H 46
#define LOAD_SCROLLBAR_W 8
#define LOAD_FOOTER_H 62
#define LOAD_CONFIRM_W 640
#define LOAD_CONFIRM_H 260

enum LoadTargetKind {
    LOAD_TARGET_NONE,
    LOAD_TARGET_LOAD,
    LOAD_TARGET_DELETE
};

typedef struct LoadTarget {

    enum LoadTargetKind kind;
    size_t index;

} LoadTarget;

typedef struct LoadMenu {

    SaveSlotList list;
    int scroll;
    LoadTarget pressed;
    _Bool is_confirming;
    size_t confirm_index;
    const char *notice;
    UiButton back_button;
    UiButton confirm_delete_button;
    UiButton confirm_cancel_button;

} LoadMenu;

static LoadMenu menu = {0};

static SDL_Rect panel_rect(void) {

    return (SDL_Rect){(int)WINDOW_SIZE_X / 2 - LOAD_PANEL_W / 2, LOAD_PANEL_Y, LOAD_PANEL_W, LOAD_PANEL_H};

}

static SDL_Rect list_rect(void) {

    const SDL_Rect panel = panel_rect();
    return (SDL_Rect){panel.x + LOAD_PAD, panel.y + LOAD_LIST_TOP, panel.w - 2 * LOAD_PAD, LOAD_LIST_H};

}

static SDL_Rect confirm_rect(void) {

    return (SDL_Rect){(int)WINDOW_SIZE_X / 2 - LOAD_CONFIRM_W / 2, (int)WINDOW_SIZE_Y / 2 - LOAD_CONFIRM_H / 2, LOAD_CONFIRM_W, LOAD_CONFIRM_H};

}

static _Bool point_in(int x, int y, SDL_Rect rect) {

    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;

}

static int max_scroll(void) {

    const int content_h = (int)menu.list.count * LOAD_ROW_STEP - LOAD_ROW_GAP;
    return content_h > LOAD_LIST_H ? content_h - LOAD_LIST_H : 0;

}

static void clamp_scroll(void) {

    if (menu.scroll > max_scroll()) menu.scroll = max_scroll();
    if (menu.scroll < 0) menu.scroll = 0;

}

static SDL_Rect row_rect(size_t index) {

    const SDL_Rect list = list_rect();
    const int scrollbar_room = max_scroll() > 0 ? LOAD_SCROLLBAR_W + 12 : 0;
    return (SDL_Rect){list.x, list.y + (int)index * LOAD_ROW_STEP - menu.scroll, list.w - scrollbar_room, LOAD_ROW_H};

}

static SDL_Rect row_button_rect(size_t index, enum LoadTargetKind kind) {

    const SDL_Rect row = row_rect(index);
    const int y = row.y + (LOAD_ROW_H - LOAD_ROW_BUTTON_H) / 2;
    const int delete_x = row.x + row.w - 16 - LOAD_ROW_BUTTON_W;
    const int x = kind == LOAD_TARGET_DELETE ? delete_x : delete_x - 12 - LOAD_ROW_BUTTON_W;
    return (SDL_Rect){x, y, LOAD_ROW_BUTTON_W, LOAD_ROW_BUTTON_H};

}

static LoadTarget target_at(int x, int y) {

    if (!point_in(x, y, list_rect())) return (LoadTarget){LOAD_TARGET_NONE, 0};

    for (size_t i = 0; i < menu.list.count; i++) {
        if (!point_in(x, y, row_rect(i))) continue;
        if (point_in(x, y, row_button_rect(i, LOAD_TARGET_DELETE))) return (LoadTarget){LOAD_TARGET_DELETE, i};
        return (LoadTarget){LOAD_TARGET_LOAD, i};
    }
    return (LoadTarget){LOAD_TARGET_NONE, 0};

}

static void refresh(void) {

    save_slots_free(&menu.list);
    menu.list = save_slots_list();
    menu.pressed = (LoadTarget){LOAD_TARGET_NONE, 0};
    clamp_scroll();

}

void load_menu_init(void) {

    menu.scroll = 0;
    menu.is_confirming = false;
    menu.notice = NULL;
    refresh();

    const SDL_Rect panel = panel_rect();
    const SDL_Rect confirm = confirm_rect();
    const int center_x = panel.x + panel.w / 2;
    menu.back_button = ui_button(center_x, panel.y + panel.h - 40 - LOAD_FOOTER_H, 300, LOAD_FOOTER_H, "Back", true);
    menu.confirm_delete_button = ui_button(confirm.x + confirm.w / 2 - 130, confirm.y + confirm.h - 40 - 56, 220, 56, "Delete", true);
    menu.confirm_cancel_button = ui_button(confirm.x + confirm.w / 2 + 130, confirm.y + confirm.h - 40 - 56, 220, 56, "Cancel", true);

}

static void open_run(size_t index) {

    const enum Scene scene = save_open_slot(menu.list.runs[index].id);
    if (scene != SCENE_NONE) {
        scene_switch(scene);
        return;
    }
    menu.notice = "That save could not be loaded. It may be damaged.";
    refresh();

}

static void confirm_input(const SDL_Event *e) {

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
        menu.is_confirming = false;
        return;
    }

    if (ui_button_event(&menu.confirm_cancel_button, e)) {
        menu.is_confirming = false;
        return;
    }

    if (ui_button_event(&menu.confirm_delete_button, e)) {
        menu.is_confirming = false;
        menu.notice = save_delete_slot(menu.list.runs[menu.confirm_index].id) ? NULL : "That save could not be deleted.";
        refresh();
    }

}

static void list_input(const SDL_Event *e) {

    if (e->type == SDL_MOUSEWHEEL) {
        menu.scroll -= e->wheel.y * LOAD_ROW_STEP;
        clamp_scroll();
        menu.pressed = (LoadTarget){LOAD_TARGET_NONE, 0};
        return;
    }

    if (e->type == SDL_MOUSEBUTTONDOWN && e->button.button == SDL_BUTTON_LEFT) {
        menu.pressed = target_at(e->button.x, e->button.y);
        return;
    }

    if (e->type == SDL_MOUSEBUTTONUP && e->button.button == SDL_BUTTON_LEFT) {
        const LoadTarget pressed = menu.pressed;
        const LoadTarget released = target_at(e->button.x, e->button.y);
        menu.pressed = (LoadTarget){LOAD_TARGET_NONE, 0};
        if (pressed.kind == LOAD_TARGET_NONE || pressed.kind != released.kind || pressed.index != released.index) return;

        menu.notice = NULL;
        if (pressed.kind == LOAD_TARGET_LOAD) {
            open_run(pressed.index);
            return;
        }
        menu.is_confirming = true;
        menu.confirm_index = pressed.index;
    }

}

void load_menu_input(SDL_Event *e) {

    if (menu.is_confirming) {
        confirm_input(e);
        return;
    }

    if (e->type == SDL_KEYDOWN && e->key.keysym.sym == SDLK_ESCAPE) {
        scene_switch(MAIN_MENU);
        return;
    }

    if (ui_button_event(&menu.back_button, e)) {
        scene_switch(MAIN_MENU);
        return;
    }

    list_input(e);

}

static void format_time(uint64_t ms, char *out, size_t out_size) {

    const time_t seconds = (time_t)(ms / 1000u);
    struct tm local;
    if (localtime_r(&seconds, &local) == NULL || strftime(out, out_size, "%Y-%m-%d %H:%M", &local) == 0) snprintf(out, out_size, "unknown");

}

static void render_row_button(size_t index, enum LoadTargetKind kind, const char *label) {

    const UiButton button = {
        .rect = row_button_rect(index, kind),
        .label = label,
        .is_enabled = true,
        .is_pressed = menu.pressed.kind == kind && menu.pressed.index == index
    };
    ui_button_render(&button);
    if (kind == LOAD_TARGET_DELETE) ui_frame(button.rect, (SDL_Color){ui_color_danger.r, ui_color_danger.g, ui_color_danger.b, 150}, 1);

}

static void render_row(size_t index) {

    const SaveRun *run = &menu.list.runs[index];
    const SDL_Rect row = row_rect(index);
    const _Bool is_complete = run->stage >= RUN_STAGE_COUNT;
    const _Bool is_hovered = !menu.is_confirming && ui_mouse_over(row) && ui_mouse_over(list_rect());

    ui_fill(row, is_hovered ? (SDL_Color){52, 50, 44, 255} : (SDL_Color){36, 38, 46, 255});
    ui_fill((SDL_Rect){row.x, row.y, 5, row.h}, is_complete ? ui_color_success : ui_color_accent);
    ui_frame(row, is_hovered ? ui_color_accent : ui_color_panel_edge, 1);

    char title[32];
    char status[32];
    char started[24];
    char played[24];
    char detail[96];
    snprintf(title, sizeof(title), "Run %u", (unsigned int)run->id);
    if (is_complete) snprintf(status, sizeof(status), "Completed");
    else snprintf(status, sizeof(status), "Stage %u / %u", (unsigned int)run->stage + 1, RUN_STAGE_COUNT);
    format_time(run->created_at, started, sizeof(started));
    format_time(run->last_played_at, played, sizeof(played));
    snprintf(detail, sizeof(detail), "Last played %s   Started %s", played, started);

    const int text_x = row.x + 26;
    const int title_y = row.y + 12;
    ui_text(UI_FONT_BUTTON, title, text_x, title_y, ui_color_text, UI_ALIGN_LEFT);
    ui_text(UI_FONT_BODY, status, text_x + 170, title_y + (ui_font_height(UI_FONT_BUTTON) - ui_font_height(UI_FONT_BODY)) / 2, is_complete ? ui_color_success : ui_color_accent, UI_ALIGN_LEFT);
    ui_text(UI_FONT_SMALL, detail, text_x, row.y + row.h - 12 - ui_font_height(UI_FONT_SMALL), ui_color_muted, UI_ALIGN_LEFT);

    render_row_button(index, LOAD_TARGET_LOAD, "Load");
    render_row_button(index, LOAD_TARGET_DELETE, "Delete");

}

static void render_scrollbar(void) {

    if (max_scroll() <= 0) return;

    const SDL_Rect list = list_rect();
    const SDL_Rect track = {list.x + list.w - LOAD_SCROLLBAR_W, list.y, LOAD_SCROLLBAR_W, list.h};
    const int content_h = (int)menu.list.count * LOAD_ROW_STEP - LOAD_ROW_GAP;
    const int thumb_h = track.h * track.h / content_h > 40 ? track.h * track.h / content_h : 40;
    const int thumb_y = track.y + (track.h - thumb_h) * menu.scroll / max_scroll();

    ui_fill(track, (SDL_Color){12, 13, 16, 255});
    ui_fill((SDL_Rect){track.x, thumb_y, track.w, thumb_h}, ui_color_panel_edge);

}

static void render_list(void) {

    const SDL_Rect list = list_rect();

    if (menu.list.count == 0) {
        ui_text(UI_FONT_BODY, "No saves yet. Start a new run from the main menu.", list.x + list.w / 2, list.y + list.h / 2 - ui_font_height(UI_FONT_BODY) / 2, ui_color_muted, UI_ALIGN_CENTER);
        return;
    }

    SDL_RenderSetClipRect(engine.renderer, &list);

    const size_t first = (size_t)(menu.scroll / LOAD_ROW_STEP);
    const size_t last = first + LOAD_VISIBLE_ROWS + 1;
    for (size_t i = first; i < menu.list.count && i <= last; i++) render_row(i);

    SDL_RenderSetClipRect(engine.renderer, NULL);
    render_scrollbar();

}

static void render_confirm(void) {

    ui_fill((SDL_Rect){0, 0, (int)WINDOW_SIZE_X, (int)WINDOW_SIZE_Y}, (SDL_Color){0, 0, 0, 160});

    const SDL_Rect box = confirm_rect();
    const SaveRun *run = &menu.list.runs[menu.confirm_index];
    ui_panel(box);
    ui_fill((SDL_Rect){box.x, box.y, box.w, 5}, ui_color_danger);

    char title[48];
    snprintf(title, sizeof(title), "Delete Run %u?", (unsigned int)run->id);
    ui_text(UI_FONT_HEADING, title, box.x + box.w / 2, box.y + 36, ui_color_text, UI_ALIGN_CENTER);
    ui_text(UI_FONT_BODY, "This save will be gone for good.", box.x + box.w / 2, box.y + 104, ui_color_muted, UI_ALIGN_CENTER);

    ui_button_render(&menu.confirm_delete_button);
    ui_frame(menu.confirm_delete_button.rect, ui_color_danger, 2);
    ui_button_render(&menu.confirm_cancel_button);

}

void load_menu_render(void) {

    ui_backdrop();

    const SDL_Rect panel = panel_rect();
    ui_panel(panel);
    ui_text(UI_FONT_HEADING, "Load game", panel.x + panel.w / 2, panel.y + 36, ui_color_accent, UI_ALIGN_CENTER);

    char count[48];
    snprintf(count, sizeof(count), menu.list.count == 1 ? "%zu save" : "%zu saves", menu.list.count);
    ui_text(UI_FONT_SMALL, count, panel.x + panel.w - LOAD_PAD, panel.y + LOAD_LIST_TOP - 30, ui_color_muted, UI_ALIGN_RIGHT);
    if (menu.notice != NULL) ui_text(UI_FONT_SMALL, menu.notice, panel.x + LOAD_PAD, panel.y + LOAD_LIST_TOP - 30, ui_color_danger, UI_ALIGN_LEFT);

    render_list();
    ui_button_render(&menu.back_button);

    if (menu.is_confirming) render_confirm();

}

void load_menu_destroy(void) {

    save_slots_free(&menu.list);
    menu.is_confirming = false;

}
