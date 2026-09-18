#include "game_loop.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "peripherals.h"
#include "scene_handler.h"
#include "scenes.h"
#include "window.h"
#include <SDL2/SDL.h>

void game_loop(void) {

    GameState *game = engine.game;

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

        switch(game->scene_state.scene) {

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
