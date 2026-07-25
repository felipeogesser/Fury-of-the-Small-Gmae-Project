#include "init.h"
#include "game_state.h"
#include "memory_arena.h"
#include "window.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void init(void) {

    srand((unsigned int)time(NULL)); 
    memory_arena_init();
    create_window();
    init_game_state();

}
