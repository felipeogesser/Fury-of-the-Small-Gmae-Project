#ifndef ENGINE_INTERNAL_H
#define ENGINE_INTERNAL_H

#include "animation_types.h"
#include "armies_types.h"
#include "battleplan_types.h"
#include "game_state_types.h"
#include "grids_types.h"
#include "main_menu_types.h"
#include "maps_types.h"
#include "player_types.h"
#include <SDL2/SDL.h>

typedef struct Engine {
    
    // core systems
    Armies *armies;
    Battleplan *battleplan;
    GameState *game;
    Grids *grids;
    MainMenu *main_menu;
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

    // animation
    Animation *animation;
    AnimationState *animationState;

    // engine state
    _Bool running;
    _Bool paused;

    // input snapshot
        //InputState input;

    // memory arenas
        //Arena permanentArena;
        //Arena frameArena;
    void *grid_memory_ptr;
    void *army_memory_ptr;
    
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

#endif