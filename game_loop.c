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

    while (*window_running) {

        float dt = game_clock_advance();
        game->delta = (!is_ai_training || engine.is_ai_bypass_disabled) ? dt : (1.0f / 60.0f);
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

            case BATTLEFIELD:

                process_peripherals(window_running);

                enum BattleStepResult step_result = battle_step_sync(shared_memory_control_base_ptr, BATTLE_TICKS_PER_DECISION, 100000);

                if (step_result == BATTLE_STEP_SHUTDOWN) {
                    *window_running = false;
                    continue;
                }

                if (step_result == BATTLE_STEP_WAITING) {
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

            default:
                break;
        }

    }

    if (is_ai_training) {
        shared_memory_control_shutdown(shared_memory_control_ptr);
    }

    scene_destroy();
    destroy_window();

}