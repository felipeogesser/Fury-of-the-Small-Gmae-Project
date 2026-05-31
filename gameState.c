#include "gameState.h"
#include "player.h"

static GameState game = {0};

GameState *pass_pointer(void) {
    return &game;
}

void update_game_state(Player *player) {
    
    game.offSetX += player->vxdt + game.LX + game.KX;
    game.offSetY += player->vydt + game.LY + game.KY;

    game.KX = 0;
    game.KY = 0;
    game.LX = 0;
    game.LY = 0;

}