#ifndef BATTLEPLAN_INSPECT_H
#define BATTLEPLAN_INSPECT_H

#include <SDL2/SDL.h>

void battleplan_inspect_reset(void);
_Bool battleplan_inspect_input(const SDL_Event *e);
_Bool battleplan_inspect_is_open(void);
SDL_Rect battleplan_inspect_rect(void);
void battleplan_inspect_render(void);

#endif
