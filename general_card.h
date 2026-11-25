#ifndef GENERAL_CARD_H
#define GENERAL_CARD_H

#include "general_types.h"
#include <SDL2/SDL.h>

const char *general_card_name(enum GeneralType type);
void general_card_render_sprite(enum GeneralType type, SDL_Rect box);
void general_card_render(enum GeneralType type, SDL_Rect rect, _Bool is_selected, _Bool is_hovered);

#endif
