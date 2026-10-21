#include "init.h"
#include "engine.h"
#include "game_clock.h"
#include "game_state.h"
#include "mailroom.h"
#include "memory_arena.h"
#include "window.h"
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>

void init(_Bool is_ai_bypass_disabled) {

    engine_init(is_ai_bypass_disabled);
    game_clock_init();
    srand((unsigned int)time(NULL)); 
    memory_arena_init();
    create_window();
    init_game_state();
    mailroom_init();
    
}
