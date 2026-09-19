#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include "armies_types.h"
#include "game_state_types.h"
#include "maps_types.h"
#include "player_types.h"

void render(Armies *armies, GameState *game, Map *map, Player *player, SDL_Renderer *renderer);

#endif