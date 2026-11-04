#ifndef UI_H
#define UI_H

#include <SDL2/SDL.h>

enum UiFont {
    UI_FONT_SMALL,
    UI_FONT_BODY,
    UI_FONT_BUTTON,
    UI_FONT_HEADING,
    UI_FONT_TITLE,
    UI_FONT_COUNT
};

enum UiAlign {
    UI_ALIGN_LEFT,
    UI_ALIGN_CENTER,
    UI_ALIGN_RIGHT
};

typedef struct UiButton {

    SDL_Rect rect;
    const char *label;
    _Bool is_enabled;
    _Bool is_pressed;

} UiButton;

extern const SDL_Color ui_color_background;
extern const SDL_Color ui_color_panel;
extern const SDL_Color ui_color_panel_edge;
extern const SDL_Color ui_color_accent;
extern const SDL_Color ui_color_text;
extern const SDL_Color ui_color_muted;
extern const SDL_Color ui_color_disabled;
extern const SDL_Color ui_color_danger;
extern const SDL_Color ui_color_success;

void ui_init(SDL_Renderer *renderer);
void ui_shutdown(void);

void ui_fill(SDL_Rect rect, SDL_Color color);
void ui_frame(SDL_Rect rect, SDL_Color color, int thickness);
void ui_panel(SDL_Rect rect);
void ui_text(enum UiFont font, const char *text, int x, int y, SDL_Color color, enum UiAlign align);
int ui_text_width(enum UiFont font, const char *text);
int ui_font_height(enum UiFont font);

UiButton ui_button(int center_x, int y, int width, int height, const char *label, _Bool is_enabled);
_Bool ui_button_event(UiButton *button, const SDL_Event *event);
void ui_button_render(const UiButton *button);

void ui_backdrop(void);

#endif
