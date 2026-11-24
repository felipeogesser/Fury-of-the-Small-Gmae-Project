#ifndef HELP_OVERLAY_H
#define HELP_OVERLAY_H

#include <SDL2/SDL.h>

void help_overlay_close(void);
_Bool help_overlay_is_open(void);
_Bool help_overlay_input(const SDL_Event *e);
void help_overlay_render(void);

#endif
