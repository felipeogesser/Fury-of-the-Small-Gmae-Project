#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>
#include "armies.h"
#include "gameState.h"
#include "maps.h"
#include "player.h"

void render(Armies *armies, GameState *game, Map *map, Player *player, SDL_Renderer *renderer);

#endif