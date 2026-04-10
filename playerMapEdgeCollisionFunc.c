#include <stdio.h>
#include "playerMapEdgeCollisionFunc.h"
#include "player.h"
#include "maps.h"
#include "gameState.h"


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
/*
PlayerMapEdgeCollision(Player *player, Map *map, GameState *game) {

    if (player->PlayerWindowPositionX <= map->MapLeftLimit - game->vydt) {
        gmae->LX = map->MapLeftLimit - player->PlayerWindowPositionX + 1;
        game->vydt = 0;
        if (gmae->LX >= -1) {
            gmae->LX = 0;
        }
    }

    if (player->PlayerWindowPositionX + player->PlayerDimensionX >= map->MapRightLimit - game->vydt) {
        gmae->LX = map->MapRightLimit - player->PlayerWindowPositionX - player->PlayerDimensionX - 1;
        game->vydt = 0;
        if (gmae->LX <= 1) {
            gmae->LX = 0;
        }
    }

    if (player->PlayerWindowPositionY <= map->MapTopLimit - game->vydt) {
        gmae->LX = map->MapTopLimit - player->PlayerWindowPositionY + 1;
        game->vydt = 0;
        if (gmae->LX >= -1) {
            gmae->LX = 0;
        }
    }

    if (player->PlayerWindowPositionY + player->PlayerDimensionY >= map->MapBottomLimit - game->vydt) {
        gmae->LX = map->MapBottomLimit - player->PlayerWindowPositionY - player->PlayerDimensionY - 1;
        game->vydt = 0;
        if (gmae->LX) {
            gmae->LX = 0;
        }
    }
}
*/