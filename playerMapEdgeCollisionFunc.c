#include "playerMapEdgeCollisionFunc.h"

void player_map_edge_collision(GameState *game, Map *map, Player *player) {

    if (player->playerPositionX <= map->mapLeftLimit - game->vxdt) {
        game->LX = -player->playerPositionX;
        game->vxdt = 0;
        /*if (game->LX >= -1) {
            game->LX = 0;
        }*/
    }

    if (player->playerPositionX + player->playerDimensionX >= map->mapRightLimit - game->vxdt) {
        game->LX = map->mapRightLimit - player->playerPositionX - player->playerDimensionX;
        game->vxdt = 0;
        /*if (game->LX <= 1) {
            game->LX = 0;
        }*/
    }

    if (player->playerPositionY <= map->mapTopLimit - game->vydt) {
        game->LY = -player->playerPositionY + 1;
        game->vydt = 0;
        /*if (game->LY >= -1) {
            game->LY = 0;
        }*/
    }

    if (player->playerPositionY + player->playerDimensionY >= map->mapBottomLimit - game->vydt) {
        game->LY = map->mapBottomLimit - player->playerPositionY - player->playerDimensionY;
        game->vydt = 0;
        /*if (game->LY <= 1) {
            game->LY = 0;
        }*/
    }
}

/*
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

*/