#include "calculateEntityMovement.h"

void calculateEntityRandomMov(Entity *get_ent, Map *map, GameState *game, Uint32 *now) {

    float dummyPositionX = get_ent->positionX;
    float dummyPositionY = get_ent->positionY;
    
    if (*now - get_ent->lastTick >= get_ent->nextMoveDelay) {
        get_ent->randomX = -1000 + rand() % 1000;
        get_ent->randomY = -1000 + rand() % 1000;
        get_ent->directionX = get_ent->randomX - get_ent->positionX;
        get_ent->directionY = get_ent->randomY - get_ent->positionY;
        get_ent->hypotenuseLengh = sqrt(get_ent->directionX*get_ent->directionX + get_ent->directionY*get_ent->directionY);
        get_ent->vectorX = get_ent->speed * get_ent->directionX/get_ent->hypotenuseLengh;
        get_ent->vectorY = get_ent->speed * get_ent->directionY/get_ent->hypotenuseLengh;
        get_ent->lastTick = *now;
    }
    
    dummyPositionX += get_ent->vectorX * game->delta - game->vxdt;
    dummyPositionY += get_ent->vectorY * game->delta - game->vydt;

    if (map->mapLeftLimit > dummyPositionX) {
        get_ent->vectorX = -get_ent->vectorX;
    }

    if (dummyPositionX > map->mapRightLimit - get_ent->dimensionX) {
        get_ent->vectorX = -get_ent->vectorX;
    }

    if (map->mapTopLimit > dummyPositionY) {
        get_ent->vectorY = -get_ent->vectorY;
    }

    if (dummyPositionY > map->mapBottomLimit - get_ent->dimensionY) {
        get_ent->vectorY = -get_ent->vectorY;
    }

    get_ent->positionX += get_ent->vectorX * game->delta - game->vxdt;
    get_ent->positionY += get_ent->vectorY * game->delta - game->vydt;



}

