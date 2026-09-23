#ifndef GAME_CLOCK_INTERNAL_H
#define GAME_CLOCK_INTERNAL_H

#include "game_clock_types.h"
#include <SDL2/SDL.h>

typedef struct GameClock {

    Uint64 FrameStart;
    Uint64 FrameEnd;
    Uint64 FrameTicks;

} GameClock;

#endif