#include "game_loop.h"
#include "battle_step.h"
#include "engine_internal.h"
#include "game_clock.h"
#include "game_state_internal.h"
#include "peripherals.h"
#include "python_process.h"
#include "scene_handler.h"
#include "scenes.h"
#include <stdio.h>

void game_loop(void) {

    GameState *game = engine.game;
    _Bool *window_running = &engine.window_running;
    _Bool is_ai_training = engine.has_program_been_initialized_with_ai_bypass;

    if (is_ai_training) {
        scene_init(BATTLEPLAN);
    } else {
        scene_init(MAIN_MENU);
    }

    game_clock_start();

    while (*window_running && !engine_cleanup_shutdown_requested()) {

        float dt = game_clock_advance();
        game->delta = (engine.is_ai_bypass_disabled) ? dt : BATTLE_TICK_DT;
        switch(game->scene_state.scene) {

            case MAIN_MENU:

                process_peripherals(window_running);

                scene_render();

                continue;

            case BATTLEPLAN:

                process_peripherals(window_running);

                scene_update(game->delta);

                scene_render();

                continue;

            case BATTLEFIELD: {

                process_peripherals(window_running);

                enum BattleStepResult step_result = battle_step_sync(shared_memory_control_base_ptr, BATTLE_TICKS_PER_DECISION, 100000);

                if (step_result == BATTLE_STEP_SHUTDOWN) {
                    *window_running = false;
                    continue;
                }

                if (step_result == BATTLE_STEP_WAITING) {
                    if (!python_process_is_alive()) {
                        fprintf(stderr, "python agent died\n");
                        *window_running = false;
                    }
                    continue;
                }

                if (step_result == BATTLE_STEP_ENDED) {
                    scene_switch(BATTLEPLAN);
                    continue;
                }

                if (step_result == BATTLE_STEP_NEW_ORDERS) {
                    battlefield_apply_orders();
                }

                scene_update(game->delta);

                scene_render();

                continue;

            }

            default:
                break;
        }

    }

}