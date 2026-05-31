#include "game_shutdown.h"
#include "memory_arena.h"

void game_shutdown(void) {

    memory_arena_free();
    
}