#include "game_loop.h"
#include "battle_controls.h"
#include "battle_step.h"
#include "battlefield.h"
#include "engine_cleanup.h"
#include "engine_internal.h"
#include "game_clock.h"
#include "game_state_internal.h"
#include "peripherals.h"
#include "python_process.h"
#include "run.h"
#include "scene_handler.h"
#include "scenes.h"
#include "tutorial.h"
#include <stdio.h>
#include <stdbool.h>
#include <time.h>

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
    uint64_t perf_ticks = 0;
    struct timespec perf_start;
    clock_gettime(CLOCK_MONOTONIC, &perf_start);
    while (*window_running && !engine_cleanup_shutdown_requested()) {

        float dt = game_clock_advance();
        game->delta = (engine.is_ai_bypass_disabled) ? dt : BATTLE_TICK_DT;
        switch(game->scene_state.scene) {

            case MAIN_MENU:
            case SETTINGS:

                process_peripherals(window_running);

                scene_render();

                continue;

            case BATTLEPLAN:
            case STAGE_MAP:

                process_peripherals(window_running);

                scene_update(game->delta);

                scene_render();

                continue;

            case BATTLEFIELD: {

                process_peripherals(window_running);

                game->delta = BATTLE_TICK_DT;
                const float time_scale = tutorial_holds_battle() ? 0.0f : battle_controls_time_scale();
                battle_step_accumulate(dt * time_scale, engine.is_ai_bypass_disabled);

                while (battle_step_tick_due() && game->scene_state.scene == BATTLEFIELD) {

                    enum BattleStepResult step_result = battle_step_sync(shared_memory_control_base_ptr, BATTLE_TICKS_PER_DECISION, 10);

                    if (step_result == BATTLE_STEP_SHUTDOWN) {
                        *window_running = false;
                        break;
                    }

                    if (step_result == BATTLE_STEP_WAITING) {
                        if (!python_process_is_alive()) {
                            fprintf(stderr, "python agent died\n");
                            *window_running = false;
                        }
                        break;
                    }

                    if (step_result == BATTLE_STEP_ENDED) {
                        if (is_ai_training) {
                            scene_switch(BATTLEPLAN);
                        } else {
                            const enum EpisodeResult result = shared_memory_control_episode_result(shared_memory_control_base_ptr);
                            if (!tutorial_record_battle(result)) run_record_result(result);
                            scene_switch(STAGE_MAP);
                        }
                        break;
                    }

                    if (step_result == BATTLE_STEP_NEW_ORDERS) {
                        battlefield_apply_orders();
                    }

                    scene_update(BATTLE_TICK_DT);
                    battle_step_consume_tick();
                    perf_ticks++;

                }

                struct timespec perf_now;
                clock_gettime(CLOCK_MONOTONIC, &perf_now);
                double perf_elapsed = (double)(perf_now.tv_sec - perf_start.tv_sec) + (double)(perf_now.tv_nsec - perf_start.tv_nsec) / 1e9;
                if (perf_elapsed >= 10.0) {
                    printf("C: %.0f ticks/s\n", (double)perf_ticks / perf_elapsed);
                    fflush(stdout);
                    perf_ticks = 0;
                    perf_start = perf_now;
                }

                if (game->scene_state.scene == BATTLEFIELD && engine.is_ai_bypass_disabled) {
                    game->delta = dt < 0.1f ? dt : 0.1f;
                    battlefield_update_view();
                    game->delta = BATTLE_TICK_DT;
                }

                if (game->scene_state.scene == BATTLEFIELD) scene_render();

                continue;

            }

            default:
                break;
        }

    }

}