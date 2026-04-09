#include <stdio.h>
#include "playerMapEdgeCollisionFunc.h"
#include "player.h"
#include "maps.h"
#include "gameState.h"

player_id = 0;
map_id = 0;

Player *player = get_player(player_id);
Map *map = get_map(map_id);


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