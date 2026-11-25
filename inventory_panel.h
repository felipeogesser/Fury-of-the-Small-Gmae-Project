#ifndef INVENTORY_PANEL_H
#define INVENTORY_PANEL_H

#include <SDL2/SDL.h>

void inventory_panel_reset(void);
_Bool inventory_panel_is_open(void);
_Bool inventory_panel_input(const SDL_Event *e);
void inventory_panel_render(void);
SDL_Rect inventory_panel_button_rect(void);

#endif
