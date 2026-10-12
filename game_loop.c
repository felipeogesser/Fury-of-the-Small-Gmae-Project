#include "game_loop.h"
#include "engine_internal.h"
#include "game_clock.h"
#include "game_state_internal.h"
#include "peripherals.h"
#include "scene_handler.h"
#include "scenes.h"
#include "window.h"

void game_loop(void) {

    GameState *game = engine.game;
    _Bool *window_running = &engine.window_running;

    scene_init(MAIN_MENU);

    game_clock_start();
    
    while (*window_running) {

        game->delta = game_clock_advance();

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
