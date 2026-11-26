#ifndef TUTORIAL_DIALOG_H
#define TUTORIAL_DIALOG_H

#include <SDL2/SDL.h>

#define TUTORIAL_DIALOG_CHOICES 2
#define TUTORIAL_DIALOG_LINES 8

enum TutorialBoxPlace {
    TUTORIAL_BOX_TOP,
    TUTORIAL_BOX_MIDDLE,
    TUTORIAL_BOX_BOTTOM
};

typedef struct TutorialDialogView {

    const char *speaker;
    const char *text;
    float typed;
    enum TutorialBoxPlace place;
    SDL_Rect target;
    _Bool dims_screen;
    const char *footer;
    const char *choices[TUTORIAL_DIALOG_CHOICES];
    unsigned int choice_count;

} TutorialDialogView;

typedef struct TutorialDialogLayout {

    SDL_Rect box;
    SDL_Rect portrait;
    int text_x;
    int text_y;
    int text_w;
    unsigned int line_count;
    unsigned int line_start[TUTORIAL_DIALOG_LINES];
    unsigned int line_length[TUTORIAL_DIALOG_LINES];
    SDL_Rect choices[TUTORIAL_DIALOG_CHOICES];
    int footer_y;

} TutorialDialogLayout;

void tutorial_dialog_layout(const TutorialDialogView *view, TutorialDialogLayout *layout);
void tutorial_dialog_render(const TutorialDialogView *view, const TutorialDialogLayout *layout);
_Bool tutorial_dialog_is_typed(const TutorialDialogView *view);

#endif
