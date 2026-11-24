#ifndef KEYBINDINGS_H
#define KEYBINDINGS_H

#include <SDL2/SDL.h>

typedef struct Keybinding {

    const char *key;
    const char *action;

} Keybinding;

typedef struct KeybindingGroup {

    const char *title;
    const Keybinding *bindings;
    unsigned int count;

} KeybindingGroup;

unsigned int keybinding_group_count(void);
const KeybindingGroup *keybinding_group(unsigned int index);
int keybindings_height(void);
void keybindings_render(SDL_Rect area);

#endif
