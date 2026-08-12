#include "gameLoop.h"
#include "armies_types.h"
#include "battalion_types.h"
#include "unit_collision.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "peripherals.h"
#include "renderer.h"
#include "scene_handler.h"
#include "scenes.h"
#include "window.h"
#include <SDL2/SDL.h>

void game_loop(void) {

    //Armies *armies = engine.armies;
    //Battalion *battalions = engine.armies->army->battalions;
    //Unit *unit = engine.armies->army->battalions->unit;
    GameState *game = engine.game;
    //Grids *grids = engine.grids;
    //Map *map = engine.map;
    //Player *player = engine.player;
    //SDL_Renderer *renderer = engine.renderer;

    scene_init(MAIN_MENU);

    Uint64 FrameStart = SDL_GetPerformanceCounter();
    engine.FrameStart = FrameStart;
    Uint64 FrameEnd;
    Uint64 FrameTicks;

    _Bool *window_running = &engine.window_running;

    while (*window_running) {

        FrameEnd = SDL_GetPerformanceCounter();
        engine.FrameEnd = FrameEnd;
        FrameTicks = SDL_GetPerformanceFrequency();
        engine.FrameTicks = FrameTicks;
        game->delta = (float)(FrameEnd - FrameStart) / (float)FrameTicks;
        FrameStart = FrameEnd;

        switch(game->scene_state) {

            case MAIN_MENU:

                process_peripherals(window_running);

                scene_render();

                continue;

            case BATTLEPLAN:

                process_peripherals(window_running);

                scene_update();

                scene_render();

                continue;

            case BATTLEFIELD:
                
                process_peripherals(window_running);

                scene_update();

                scene_render();

                continue;

            default:
                break;
        }

    }
    
    scene_destroy();
    destroy_window();

}
