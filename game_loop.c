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

                if (is_ai_training) {

                    if (pending_step == 0) {
                        pending_step = shared_memory_control_publish_step(shared_memory_control_ptr);
                    }

                    enum SharedMemoryWaitResult wait_result = shared_memory_control_wait_for_python(shared_memory_control_ptr, pending_step, 100000);

                    if (wait_result == SHARED_MEMORY_WAIT_SHUTDOWN) {
                        *window_running = false;
                        continue;
                    }

                    if (wait_result == SHARED_MEMORY_WAIT_TIMEOUT) {
                        continue;
                    }

                    pending_step = 0;

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