#include "playerMapEdgeCollisionFunc.h"

void player_map_edge_collision(GameState *game, Map *map, Player *player) {

    if (player->playerPositionX <= map->mapLeftLimit - player->vxdt) {
        game->LX = -player->playerPositionX;
        player->vxdt = 0;
        /*if (game->LX >= -1) {
            game->LX = 0;
        }*/
    }

    if (player->playerPositionX + player->playerDimensionX >= map->mapRightLimit - player->vxdt) {
        game->LX = map->mapRightLimit - player->playerPositionX - player->playerDimensionX;
        player->vxdt = 0;
        /*if (game->LX <= 1) {
            game->LX = 0;
        }*/
    }

    if (player->playerPositionY <= map->mapTopLimit - player->vydt) {
        game->LY = -player->playerPositionY + 1;
        player->vydt = 0;
        /*if (game->LY >= -1) {
            game->LY = 0;
        }*/
    }

    if (player->playerPositionY + player->playerDimensionY >= map->mapBottomLimit - player->vydt) {
        game->LY = map->mapBottomLimit - player->playerPositionY - player->playerDimensionY;
        player->vydt = 0;
        /*if (game->LY <= 1) {
            game->LY = 0;
        }*/
    }
}

/*
if (player->playerWindowPositionX <= map->mapLeftLimit - player->vxdt) {
        game->LX = map->mapLeftLimit - player->playerWindowPositionX + 1;
        player->vxdt = 0;
        if (game->LX >= -1) {
            game->LX = 0;
        }
    }

    if (player->playerWindowPositionX + player->playerDimensionX >= map->mapRightLimit - player->vxdt) {
        game->LX = map->mapRightLimit - player->playerWindowPositionX - player->playerDimensionX - 1;
        player->vxdt = 0;
        if (game->LX <= 1) {
            game->LX = 0;
        }
    }

    if (player->playerWindowPositionY <= map->mapTopLimit - player->vydt) {
        game->LY = map->mapTopLimit - player->playerWindowPositionY + 1;
        player->vydt = 0;
        if (game->LY >= -1) {
            game->LY = 0;
        }
    }

    if (player->playerWindowPositionY + player->playerDimensionY >= map->mapBottomLimit - player->vydt) {
        game->LY = map->mapBottomLimit - player->playerWindowPositionY - player->playerDimensionY - 1;
        player->vydt = 0;
        if (game->LY <= 1) {
            game->LY = 0;
        }
    }
}

*/