#include "game_state.h"
#include "game_state_internal.h"
#include "engine_internal.h"

static GameState game = {0};

void init_game_state(void) {
    
    engine.game = &game;

}

void update_game_state(void) {
    
    game.KX = 0;
    game.KY = 0;
    game.LX = 0;
    game.LY = 0;

}