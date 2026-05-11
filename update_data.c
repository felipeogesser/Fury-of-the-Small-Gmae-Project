#include "update_data.h"

void update_player(GameState *game, Player *player) {
    
    player->playerPositionX += game->vxdt + game->LX + game->KX;
    player->playerPositionY += game->vydt + game->LY + game->KY;

}

void update_units(Entity *entities, GameState *game) {
    
    for (unsigned int i = 0; i < game->entities_created_count; i++) {
        entities[i].positionX += entities[i].vectorX * game->delta;
        entities[i].positionY += entities[i].vectorY * game->delta;
    }

}

void update_game_state(GameState *game) {
    
    game->offSetX += game->vxdt + game->LX + game->KX;
    game->offSetY += game->vydt + game->LY + game->KY;

    game->KX = 0;
    game->KY = 0;
    game->LX = 0;
    game->LY = 0;

}

void update_game_data(Entity *entities, GameState *game, Player *player) {
    
    update_player(game, player);

    update_units(entities, game);

    update_game_state(game);

}