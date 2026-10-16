#ifndef GAME_CLOCK_INTERNAL_H
#define GAME_CLOCK_INTERNAL_H

#include "game_clock_types.h"
#include <SDL2/SDL.h>

typedef struct GameClock {

    Uint64 absolute_start_time;
    Uint64 frame_start;
    Uint64 frame_end;
    Uint64 frequency;
    Uint64 frame_count;

} GameClock;

#endif