#ifndef BATTLEFIELD_POWER_BAR_H
#define BATTLEFIELD_POWER_BAR_H

#include "table_internal.h"
#include <SDL2/SDL.h>

SDL_Rect battlefield_power_bar_rect(void);
void battlefield_power_bar_render(const Table *left_tables, const Table *right_tables, unsigned int battalion_count, unsigned int battalion_size, SDL_Color left_color, SDL_Color right_color);

#endif
