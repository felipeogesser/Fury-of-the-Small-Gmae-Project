#include "game_clock.h"
#include "game_clock_internal.h"
#include "engine_internal.h"
#include <SDL2/SDL.h>

GameClock clock = {0};

void game_clock_init(void) {

    clock.absolute_start_time = SDL_GetPerformanceCounter();
    engine.clock = &clock;

}

void game_clock_start(void) {

    clock.frame_start = SDL_GetPerformanceCounter();

}

float game_clock_advance(void) {

    Uint64 frame_end = SDL_GetPerformanceCounter()
    Uint64 frame_ticks = SDL_GetPerformanceFrequency();
    float delta_time = (float)(frame_end - clock.frame_start) / (float)frame_ticks;
    clock.frame_start = frame_end;
    clock.frame_end = frame_end;
    return delta_time;

}
