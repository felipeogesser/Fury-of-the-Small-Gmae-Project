#ifndef ENGINE_H
#define ENGINE_H

#include "armies.h"
#include "gameState.h"
#include "grids.h"
#include "maps.h"
#include "player.h"
#include <SDL2/SDL.h>


typedef struct Engine {
    
    // core systems
    Armies *armies;
    GameState *game;
    Grids *grids;
    Map *map;
    Player *player;

    // platform layer
    SDL_Window *window;
    SDL_Renderer *renderer;
    _Bool window_running;
    
    // timing
    float delta;
    Uint64 FrameStart;
    Uint64 FrameEnd;
    Uint64 FrameTicks;
    Uint64 perfFreq;

    // engine state
    _Bool running;
    _Bool paused;

    // input snapshot
        //InputState input;

    // memory arenas
        //Arena permanentArena;
        //Arena frameArena;
    void *grid_memory;
    void *army_memory;
    
    // rendering systems
        //Camera camera;

    // debugging/profiling
        //DebugInfo debug;

    // audio
        //AudioSystem audio;

    // threading/jobs
        //JobSystem jobs;

} Engine;

extern Engine engine;

//void game_context(void);

#endif