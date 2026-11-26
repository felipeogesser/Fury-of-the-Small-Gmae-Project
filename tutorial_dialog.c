#include "tutorial_dialog.h"
#include "engine_internal.h"
#include "sprites_internal.h"
#include "ui.h"
#include "window_settings.h"
#include <math.h>
#include <stdbool.h>
#include <string.h>

#define DIALOG_W 1000
#define DIALOG_PAD 22
#define DIALOG_PORTRAIT 116
#define DIALOG_GAP 22
#define DIALOG_TOP_Y 100
#define DIALOG_BOTTOM_EDGE 745
#define DIALOG_CHOICE_H 42
#define DIALOG_CHOICE_GAP 16
#define DIALOG_LINE_BUFFER 256
#define DIALOG_FRAME_MS 140

static const SDL_Rect portrait_crop = {38, 33, 26, 26};
static const SDL_Color speaker_color = {92, 150, 214, 255};

static void copy_span(char *buffer, const char *text, unsigned int start, unsigned int length) {

    if (length >= DIALOG_LINE_BUFFER) length = DIALOG_LINE_BUFFER - 1;
    memcpy(buffer, text + start, length);
    buffer[length] = '\0';

}

static void wrap_text(const char *text, int width, TutorialDialogLayout *layout) {

    char buffer[DIALOG_LINE_BUFFER];
    const unsigned int total = (unsigned int)strlen(text);
    unsigned int line_start = 0;
    layout->line_count = 0;

    while (line_start < total && layout->line_count < TUTORIAL_DIALOG_LINES) {

        unsigned int best_end = 0;
        unsigned int cursor = line_start;
        while (cursor <= total) {
            if (cursor == total || text[cursor] == ' ' || text[cursor] == '\n') {
                copy_span(buffer, text, line_start, cursor - line_start);
                if (best_end != 0 && ui_text_width(UI_FONT_BODY, buffer) > width) break;
                best_end = cursor;
                if (cursor == total || text[cursor] == '\n') break;
            }
            cursor++;
        }

        if (best_end <= line_start) best_end = cursor < total ? cursor : total;
        layout->line_start[layout->line_count] = line_start;
        layout->line_length[layout->line_count] = best_end - line_start;
        layout->line_count++;
        line_start = best_end;
        while (line_start < total && (text[line_start] == ' ' || text[line_start] == '\n')) line_start++;

    }

}

_Bool tutorial_dialog_is_typed(const TutorialDialogView *view) {

    return view->typed >= (float)strlen(view->text);

}

void tutorial_dialog_layout(const TutorialDialogView *view, TutorialDialogLayout *layout) {

    memset(layout, 0, sizeof(*layout));

    const int text_w = DIALOG_W - 2 * DIALOG_PAD - DIALOG_PORTRAIT - DIALOG_GAP;
    wrap_text(view->text, text_w, layout);

    const int speaker_h = ui_font_height(UI_FONT_SMALL);
    const int line_h = ui_font_height(UI_FONT_BODY) + 4;
    const int footer_h = view->choice_count > 0 ? DIALOG_CHOICE_H : ui_font_height(UI_FONT_SMALL);
    int content_h = speaker_h + 8 + (int)layout->line_count * line_h + 14 + footer_h;
    if (content_h < DIALOG_PORTRAIT) content_h = DIALOG_PORTRAIT;
    const int h = DIALOG_PAD + content_h + DIALOG_PAD;

    int y = DIALOG_TOP_Y;
    if (view->place == TUTORIAL_BOX_MIDDLE) y = ((int)WINDOW_SIZE_Y - h) / 2;
    if (view->place == TUTORIAL_BOX_BOTTOM) y = DIALOG_BOTTOM_EDGE - h;

    layout->box = (SDL_Rect){((int)WINDOW_SIZE_X - DIALOG_W) / 2, y, DIALOG_W, h};
    layout->portrait = (SDL_Rect){layout->box.x + DIALOG_PAD, y + DIALOG_PAD, DIALOG_PORTRAIT, DIALOG_PORTRAIT};
    layout->text_x = layout->portrait.x + DIALOG_PORTRAIT + DIALOG_GAP;
    layout->text_y = y + DIALOG_PAD + speaker_h + 8;
    layout->text_w = text_w;
    layout->footer_y = y + h - DIALOG_PAD - footer_h;

    int choice_x = layout->text_x;
    for (unsigned int i = 0; i < view->choice_count && i < TUTORIAL_DIALOG_CHOICES; i++) {
        const int w = ui_text_width(UI_FONT_BODY, view->choices[i]) + 64;
        layout->choices[i] = (SDL_Rect){choice_x, layout->footer_y, w, DIALOG_CHOICE_H};
        choice_x += w + DIALOG_CHOICE_GAP;
    }

}

static void render_dim(SDL_Rect target) {

    const SDL_Color shade = {0, 0, 0, 150};
    const int screen_w = (int)WINDOW_SIZE_X;
    const int screen_h = (int)WINDOW_SIZE_Y;

    if (target.w <= 0 || target.h <= 0) {
        ui_fill((SDL_Rect){0, 0, screen_w, screen_h}, shade);
        return;
    }

    const SDL_Rect hole = {target.x - 10, target.y - 10, target.w + 20, target.h + 20};
    ui_fill((SDL_Rect){0, 0, screen_w, hole.y}, shade);
    ui_fill((SDL_Rect){0, hole.y + hole.h, screen_w, screen_h - hole.y - hole.h}, shade);
    ui_fill((SDL_Rect){0, hole.y, hole.x, hole.h}, shade);
    ui_fill((SDL_Rect){hole.x + hole.w, hole.y, screen_w - hole.x - hole.w, hole.h}, shade);

}

static void render_target(SDL_Rect target) {

    if (target.w <= 0 || target.h <= 0) return;

    const float pulse = 0.5f + 0.5f * sinf((float)SDL_GetTicks() * 0.006f);
    const int grow = 10 + (int)lroundf(pulse * 6.0f);
    const SDL_Rect outer = {target.x - grow, target.y - grow, target.w + 2 * grow, target.h + 2 * grow};
    ui_frame(outer, (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, (Uint8)(140.0f + 115.0f * pulse)}, 3);
    ui_frame((SDL_Rect){outer.x - 5, outer.y - 5, outer.w + 10, outer.h + 10}, (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, (Uint8)(60.0f * pulse)}, 2);

}

static void render_portrait(SDL_Rect box) {

    ui_fill(box, (SDL_Color){20, 22, 28, 255});
    ui_fill((SDL_Rect){box.x, box.y + box.h - box.h / 3, box.w, box.h / 3}, (SDL_Color){speaker_color.r, speaker_color.g, speaker_color.b, 40});

    const Sprite *sprite = &engine.sprite_pack->sprite[SPRITE_GUARDSMEN][IDLE];
    if (sprite->texture != NULL && sprite->frames_count > 0) {
        const int frame_w = sprite->width / sprite->frames_count;
        const unsigned int frame = (SDL_GetTicks() / DIALOG_FRAME_MS) % sprite->frames_count;
        const SDL_Rect crop = {(int)frame * frame_w + portrait_crop.x, portrait_crop.y, portrait_crop.w, portrait_crop.h};
        SDL_RenderCopy(engine.renderer, sprite->texture, &crop, &box);
    }

    ui_frame(box, ui_color_panel_edge, 2);

}

static void render_text(const TutorialDialogView *view, const TutorialDialogLayout *layout) {

    char buffer[DIALOG_LINE_BUFFER];
    const int line_h = ui_font_height(UI_FONT_BODY) + 4;
    const unsigned int typed = view->typed <= 0.0f ? 0u : (unsigned int)view->typed;

    SDL_Rect previous_clip;
    SDL_RenderGetClipRect(engine.renderer, &previous_clip);
    const _Bool had_clip = SDL_RenderIsClipEnabled(engine.renderer);

    for (unsigned int i = 0; i < layout->line_count; i++) {

        const unsigned int start = layout->line_start[i];
        const unsigned int length = layout->line_length[i];
        if (typed <= start) break;

        const int y = layout->text_y + (int)i * line_h;
        copy_span(buffer, view->text, start, length);

        if (typed >= start + length) {
            ui_text(UI_FONT_BODY, buffer, layout->text_x, y, ui_color_text, UI_ALIGN_LEFT);
            continue;
        }

        char prefix[DIALOG_LINE_BUFFER];
        copy_span(prefix, view->text, start, typed - start);
        const SDL_Rect clip = {layout->text_x, y, ui_text_width(UI_FONT_BODY, prefix), line_h};
        SDL_RenderSetClipRect(engine.renderer, &clip);
        ui_text(UI_FONT_BODY, buffer, layout->text_x, y, ui_color_text, UI_ALIGN_LEFT);
        SDL_RenderSetClipRect(engine.renderer, had_clip ? &previous_clip : NULL);

    }

}

static void render_footer(const TutorialDialogView *view, const TutorialDialogLayout *layout) {

    if (!tutorial_dialog_is_typed(view)) return;

    for (unsigned int i = 0; i < view->choice_count && i < TUTORIAL_DIALOG_CHOICES; i++) {

        const SDL_Rect rect = layout->choices[i];
        const _Bool is_hovered = ui_mouse_over(rect);
        ui_fill(rect, is_hovered ? (SDL_Color){62, 58, 48, 255} : (SDL_Color){44, 46, 56, 255});
        ui_frame(rect, is_hovered ? ui_color_accent : ui_color_panel_edge, 2);

        char label[DIALOG_LINE_BUFFER];
        SDL_snprintf(label, sizeof(label), "%u. %s", i + 1, view->choices[i]);
        ui_text(UI_FONT_BODY, label, rect.x + rect.w / 2, rect.y + (rect.h - ui_font_height(UI_FONT_BODY)) / 2, is_hovered ? ui_color_accent : ui_color_text, UI_ALIGN_CENTER);

    }

    if (view->choice_count == 0 && view->footer != NULL) {
        const float pulse = 0.6f + 0.4f * sinf((float)SDL_GetTicks() * 0.005f);
        const SDL_Color color = {ui_color_muted.r, ui_color_muted.g, ui_color_muted.b, (Uint8)(255.0f * pulse)};
        ui_text(UI_FONT_SMALL, view->footer, layout->box.x + layout->box.w - DIALOG_PAD, layout->footer_y, color, UI_ALIGN_RIGHT);
    }

}

void tutorial_dialog_render(const TutorialDialogView *view, const TutorialDialogLayout *layout) {

    if (view->dims_screen) render_dim(view->target);
    render_target(view->target);

    const SDL_Rect box = layout->box;
    ui_fill((SDL_Rect){box.x + 8, box.y + 10, box.w, box.h}, (SDL_Color){0, 0, 0, 120});
    ui_fill(box, (SDL_Color){ui_color_panel.r, ui_color_panel.g, ui_color_panel.b, 250});
    ui_fill((SDL_Rect){box.x, box.y, box.w, 5}, ui_color_accent);
    ui_frame(box, ui_color_panel_edge, 2);

    render_portrait(layout->portrait);
    ui_text(UI_FONT_SMALL, view->speaker, layout->text_x, box.y + DIALOG_PAD, speaker_color, UI_ALIGN_LEFT);
    render_text(view, layout);
    render_footer(view, layout);

}
