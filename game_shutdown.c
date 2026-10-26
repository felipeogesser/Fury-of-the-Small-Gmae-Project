#include "game_shutdown.h"
#include "engine_cleanup.h"
#include "memory_arena.h"
#include "scenes.h"
#include "window.h"

void game_shutdown(void) {

    python_process_stop(shared_memory_control_base_ptr, 5000);
    memory_arena_free();    

}