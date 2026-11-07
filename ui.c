#include "ui.h"
#include "window_settings.h"
#include <SDL2/SDL_ttf.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#define UI_TEXT_CACHE 96
#define UI_TEXT_MAX 128
#define UI_HILL_STEPS 64

typedef struct UiCachedText {

    char text[UI_TEXT_MAX];
    enum UiFont font;
    SDL_Color color;
    SDL_Texture *texture;
    int w;
    int h;
    unsigned int last_used;

} UiCachedText;

const SDL_Color ui_color_background = {22, 24, 30, 255};
const SDL_Color ui_color_panel = {36, 39, 48, 235};
const SDL_Color ui_color_panel_edge = {74, 78, 92, 255};
const SDL_Color ui_color_accent = {214, 172, 84, 255};
const SDL_Color ui_color_text = {236, 230, 218, 255};
const SDL_Color ui_color_muted = {146, 150, 162, 255};
const SDL_Color ui_color_disabled = {88, 91, 102, 255};
const SDL_Color ui_color_danger = {196, 84, 76, 255};
const SDL_Color ui_color_success = {116, 176, 104, 255};

static const struct { const char *path; int size; } font_files[UI_FONT_COUNT] = {
    [UI_FONT_SMALL] = {"assets/fonts/DejaVuSans.ttf", 18},
    [UI_FONT_BODY] = {"assets/fonts/DejaVuSans.ttf", 24},
    [UI_FONT_BUTTON] = {"assets/fonts/DejaVuSans-Bold.ttf", 26},
    [UI_FONT_HEADING] = {"assets/fonts/DejaVuSerif-Bold.ttf", 44},
    [UI_FONT_TITLE] = {"assets/fonts/DejaVuSerif-Bold.ttf", 96},
};

static SDL_Renderer *ui_renderer = NULL;
static TTF_Font *fonts[UI_FONT_COUNT] = {0};
static UiCachedText cache[UI_TEXT_CACHE] = {0};
static unsigned int cache_clock = 0;

static UiCachedText *cached_text(enum UiFont font, const char *text, SDL_Color color);
static _Bool point_in_rect(int x, int y, SDL_Rect rect);
static void mouse_position(int *x, int *y);
static void gradient(SDL_Rect rect, SDL_Color top, SDL_Color bottom);
static void hills(float base_y, float amplitude, float frequency, float phase, SDL_Color color);

void ui_init(SDL_Renderer *renderer) {

    ui_renderer = renderer;
    if (TTF_Init() == -1) {
        fprintf(stderr, "TTF_Init failed: %s\n", TTF_GetError());
        return;
    }

    for (unsigned int i = 0; i < UI_FONT_COUNT; i++) {

        fonts[i] = TTF_OpenFont(font_files[i].path, font_files[i].size);
        if (!fonts[i]) fprintf(stderr, "could not open font %s: %s\n", font_files[i].path, TTF_GetError());

    }

}

void ui_shutdown(void) {

    for (unsigned int i = 0; i < UI_TEXT_CACHE; i++) {

        if (cache[i].texture) SDL_DestroyTexture(cache[i].texture);
        cache[i].texture = NULL;

    }

    for (unsigned int i = 0; i < UI_FONT_COUNT; i++) {

        if (fonts[i]) TTF_CloseFont(fonts[i]);
        fonts[i] = NULL;

    }

    TTF_Quit();

}

void ui_fill(SDL_Rect rect, SDL_Color color) {

    SDL_SetRenderDrawColor(ui_renderer, color.r, color.g, color.b, color.a);
    SDL_RenderFillRect(ui_renderer, &rect);

}

void ui_frame(SDL_Rect rect, SDL_Color color, int thickness) {

    SDL_SetRenderDrawColor(ui_renderer, color.r, color.g, color.b, color.a);
    for (int i = 0; i < thickness; i++) {

        SDL_Rect edge = {rect.x + i, rect.y + i, rect.w - 2 * i, rect.h - 2 * i};
        SDL_RenderDrawRect(ui_renderer, &edge);

    }

}

void ui_panel(SDL_Rect rect) {

    SDL_Rect shadow = {rect.x + 6, rect.y + 8, rect.w, rect.h};
    ui_fill(shadow, (SDL_Color){0, 0, 0, 90});
    ui_fill(rect, ui_color_panel);
    ui_frame(rect, ui_color_panel_edge, 2);

}

void ui_text(enum UiFont font, const char *text, int x, int y, SDL_Color color, enum UiAlign align) {

    if (color.a == 0) return;

    UiCachedText *entry = cached_text(font, text, (SDL_Color){color.r, color.g, color.b, 255});
    if (!entry) return;

    SDL_Rect target = {x, y, entry->w, entry->h};
    if (align == UI_ALIGN_CENTER) target.x -= entry->w / 2;
    if (align == UI_ALIGN_RIGHT) target.x -= entry->w;
    SDL_SetTextureAlphaMod(entry->texture, color.a);
    SDL_RenderCopy(ui_renderer, entry->texture, NULL, &target);

}

int ui_text_width(enum UiFont font, const char *text) {

    int w = 0;
    if (fonts[font] && TTF_SizeUTF8(fonts[font], text, &w, NULL) == 0) return w;
    return 0;

}

int ui_font_height(enum UiFont font) {

    return fonts[font] ? TTF_FontHeight(fonts[font]) : 0;

}

_Bool ui_mouse_over(SDL_Rect rect) {

    int mouse_x = 0;
    int mouse_y = 0;
    mouse_position(&mouse_x, &mouse_y);
    return point_in_rect(mouse_x, mouse_y, rect);

}

UiButton ui_button(int center_x, int y, int width, int height, const char *label, _Bool is_enabled) {

    UiButton button = {
        .rect = {center_x - width / 2, y, width, height},
        .label = label,
        .is_enabled = is_enabled,
        .is_pressed = false
    };
    return button;

}

_Bool ui_button_event(UiButton *button, const SDL_Event *event) {

    if (!button->is_enabled) {
        button->is_pressed = false;
        return false;
    }

    if (event->type == SDL_MOUSEBUTTONDOWN && event->button.button == SDL_BUTTON_LEFT) {
        button->is_pressed = point_in_rect(event->button.x, event->button.y, button->rect);
        return false;
    }

    if (event->type == SDL_MOUSEBUTTONUP && event->button.button == SDL_BUTTON_LEFT) {
        _Bool was_pressed = button->is_pressed;
        button->is_pressed = false;
        return was_pressed && point_in_rect(event->button.x, event->button.y, button->rect);
    }

    return false;

}

void ui_button_render(const UiButton *button) {

    int mouse_x = 0;
    int mouse_y = 0;
    mouse_position(&mouse_x, &mouse_y);
    const _Bool is_hovered = button->is_enabled && point_in_rect(mouse_x, mouse_y, button->rect);
    const int sink = (button->is_pressed && is_hovered) ? 3 : 0;

    SDL_Rect body = {button->rect.x, button->rect.y + sink, button->rect.w, button->rect.h};
    SDL_Rect shadow = {button->rect.x, button->rect.y + 5, button->rect.w, button->rect.h};
    ui_fill(shadow, (SDL_Color){0, 0, 0, 110});

    SDL_Color fill = is_hovered ? (SDL_Color){62, 58, 48, 255} : (SDL_Color){44, 46, 56, 255};
    if (!button->is_enabled) fill = (SDL_Color){34, 36, 42, 255};
    ui_fill(body, fill);

    SDL_Color edge = button->is_enabled ? (is_hovered ? ui_color_accent : ui_color_panel_edge) : (SDL_Color){52, 54, 62, 255};
    ui_frame(body, edge, 2);
    if (is_hovered) {
        SDL_Rect inner = {body.x + 4, body.y + 4, body.w - 8, body.h - 8};
        ui_frame(inner, (SDL_Color){ui_color_accent.r, ui_color_accent.g, ui_color_accent.b, 70}, 1);
    }

    SDL_Color label = button->is_enabled ? (is_hovered ? ui_color_accent : ui_color_text) : ui_color_disabled;
    const int text_y = body.y + (body.h - ui_font_height(UI_FONT_BUTTON)) / 2;
    ui_text(UI_FONT_BUTTON, button->label, body.x + body.w / 2, text_y, label, UI_ALIGN_CENTER);

}

void ui_backdrop(void) {

    const SDL_Rect sky = {0, 0, (int)WINDOW_SIZE_X, (int)WINDOW_SIZE_Y};
    gradient(sky, (SDL_Color){18, 22, 38, 255}, (SDL_Color){122, 74, 52, 255});

    hills(WINDOW_SIZE_Y * 0.62f, 60.0f, 2.1f, 0.4f, (SDL_Color){52, 44, 52, 255});
    hills(WINDOW_SIZE_Y * 0.72f, 48.0f, 3.3f, 1.7f, (SDL_Color){36, 34, 42, 255});
    hills(WINDOW_SIZE_Y * 0.83f, 36.0f, 4.7f, 3.1f, (SDL_Color){22, 22, 28, 255});

}

static UiCachedText *cached_text(enum UiFont font, const char *text, SDL_Color color) {

    if (!fonts[font] || !text || text[0] == '\0') return NULL;
    cache_clock++;

    UiCachedText *oldest = &cache[0];
    for (unsigned int i = 0; i < UI_TEXT_CACHE; i++) {

        UiCachedText *entry = &cache[i];
        if (entry->texture && entry->font == font && entry->color.r == color.r && entry->color.g == color.g &&
            entry->color.b == color.b && entry->color.a == color.a && strncmp(entry->text, text, UI_TEXT_MAX) == 0) {
            entry->last_used = cache_clock;
            return entry;
        }
        if (!entry->texture || entry->last_used < oldest->last_used) oldest = entry;
        if (!oldest->texture) break;

    }

    SDL_Surface *surface = TTF_RenderUTF8_Blended(fonts[font], text, color);
    if (!surface) return NULL;
    SDL_Texture *texture = SDL_CreateTextureFromSurface(ui_renderer, surface);
    const int w = surface->w;
    const int h = surface->h;
    SDL_FreeSurface(surface);
    if (!texture) return NULL;

    if (oldest->texture) SDL_DestroyTexture(oldest->texture);
    snprintf(oldest->text, UI_TEXT_MAX, "%s", text);
    oldest->font = font;
    oldest->color = color;
    oldest->texture = texture;
    oldest->w = w;
    oldest->h = h;
    oldest->last_used = cache_clock;
    return oldest;

}

static _Bool point_in_rect(int x, int y, SDL_Rect rect) {

    return x >= rect.x && x < rect.x + rect.w && y >= rect.y && y < rect.y + rect.h;

}

static void mouse_position(int *x, int *y) {

    int window_x = 0;
    int window_y = 0;
    SDL_GetMouseState(&window_x, &window_y);
    float logical_x = (float)window_x;
    float logical_y = (float)window_y;
    SDL_RenderWindowToLogical(ui_renderer, window_x, window_y, &logical_x, &logical_y);
    *x = (int)logical_x;
    *y = (int)logical_y;

}

static void gradient(SDL_Rect rect, SDL_Color top, SDL_Color bottom) {

    const float x0 = (float)rect.x;
    const float y0 = (float)rect.y;
    const float x1 = (float)(rect.x + rect.w);
    const float y1 = (float)(rect.y + rect.h);
    const SDL_Vertex vertices[4] = {
        {{x0, y0}, top, {0.0f, 0.0f}},
        {{x1, y0}, top, {0.0f, 0.0f}},
        {{x1, y1}, bottom, {0.0f, 0.0f}},
        {{x0, y1}, bottom, {0.0f, 0.0f}},
    };
    const int indices[6] = {0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(ui_renderer, NULL, vertices, 4, indices, 6);

}

static void hills(float base_y, float amplitude, float frequency, float phase, SDL_Color color) {

    SDL_Vertex vertices[(UI_HILL_STEPS + 1) * 2];
    int indices[UI_HILL_STEPS * 6];
    for (int i = 0; i <= UI_HILL_STEPS; i++) {

        const float t = (float)i / (float)UI_HILL_STEPS;
        const float x = t * WINDOW_SIZE_X;
        const float wave = sinf(t * frequency * 6.2831853f + phase) * 0.65f + sinf(t * frequency * 15.7f + phase * 2.3f) * 0.35f;
        vertices[i * 2] = (SDL_Vertex){{x, base_y - amplitude * wave}, color, {0.0f, 0.0f}};
        vertices[i * 2 + 1] = (SDL_Vertex){{x, WINDOW_SIZE_Y}, color, {0.0f, 0.0f}};

    }

    for (int i = 0; i < UI_HILL_STEPS; i++) {

        const int a = i * 2;
        indices[i * 6 + 0] = a;
        indices[i * 6 + 1] = a + 1;
        indices[i * 6 + 2] = a + 2;
        indices[i * 6 + 3] = a + 2;
        indices[i * 6 + 4] = a + 1;
        indices[i * 6 + 5] = a + 3;

    }

    SDL_RenderGeometry(ui_renderer, NULL, vertices, (UI_HILL_STEPS + 1) * 2, indices, UI_HILL_STEPS * 6);

}
