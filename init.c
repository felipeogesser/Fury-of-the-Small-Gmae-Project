#include "init.h"
#include "baking.h"
#include "engine.h"
#include "engine_cleanup.h"
#include "engine_internal.h"
#include "game_clock.h"
#include "game_state.h"
#include "mailroom.h"
#include "memory_arena.h"
#include "python_process.h"
#include "shared_memory_control.h"
#include "window.h"
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void init(_Bool is_ai_bypass_disabled) {

    engine_init(is_ai_bypass_disabled);
    game_clock_init();
    srand((unsigned int)time(NULL));
    memory_arena_init();
    engine_cleanup_install();
    data_baking_shared_memory();
    PythonProcessConfig python_config = {
        .python_path = ".venv/bin/python",
        .script_path = "python/agent.py",
        .checkpoint_path = "checkpoints/latest.pt",
        .shared_name = memory_arena_shared_name(),
        .is_training = engine.has_program_been_initialized_with_ai_bypass
    };
    python_process_spawn(&python_config);
    create_window();
    data_baking_assets();
    init_game_state();
    mailroom_init();
    enum PythonReadyResult ready = python_process_wait_ready(shared_memory_control_base_ptr, 30000);
    if (ready != PYTHON_READY) {
        fprintf(stderr, "python agent failed to start\n");
        exit(EXIT_FAILURE);
    }
    memory_arena_unlink_name();

}