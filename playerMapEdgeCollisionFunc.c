#include "playerMapEdgeCollisionFunc.h"

void PlayerMapEdgeCollision(Player *player, Map *map, GameState *game) {

    if (player->playerWindowPositionX <= map->mapLeftLimit - game->vxdt) {
        game->LX = map->mapLeftLimit - player->playerWindowPositionX + 1;
        game->vxdt = 0;
        if (game->LX >= -1) {
            game->LX = 0;
        }
    }

    if (player->playerWindowPositionX + player->playerDimensionX >= map->mapRightLimit - game->vxdt) {
        game->LX = map->mapRightLimit - player->playerWindowPositionX - player->playerDimensionX - 1;
        game->vxdt = 0;
        if (game->LX <= 1) {
            game->LX = 0;
        }
    }

    if (player->playerWindowPositionY <= map->mapTopLimit - game->vydt) {
        game->LY = map->mapTopLimit - player->playerWindowPositionY + 1;
        game->vydt = 0;
        if (game->LY >= -1) {
            game->LY = 0;
        }
    }

    if (player->playerWindowPositionY + player->playerDimensionY >= map->mapBottomLimit - game->vydt) {
        game->LY = map->mapBottomLimit - player->playerWindowPositionY - player->playerDimensionY - 1;
        game->vydt = 0;
        if (game->LY <= 1) {
            game->LY = 0;
        }
    }
}
