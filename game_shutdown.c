#include "game_shutdown.h"
#include "memory_arena.h"
#include "python_process.h"

void game_shutdown(void) {

    python_process_stop(shared_memory_control_base_ptr, 5000);
    memory_arena_free();    

}