#include "gameState.h"

static GameState gameState = {0};

GameState *pass_pointer(void) {
    return &gameState;
}