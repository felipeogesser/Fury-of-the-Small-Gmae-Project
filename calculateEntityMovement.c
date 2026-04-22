#include "calculateEntityMovement.h"

void calculateEntityRandomMov(Entity *get_ent, Map *map, GameState *game, Uint32 now1, Uint32 now2) {

    float delta = game->delta;
    float vectorX = get_ent->vectorX;
    float vectorY = get_ent->vectorY;
    float dummyPositionX = get_ent->positionX;
    float dummyPositionY = get_ent->positionY;

    if (now2 - get_ent->lastTick >= get_ent->nextMoveDelay) {
        Uint32 seedX = now1 ^ ((Uint32)get_ent->id * 1103515245u);
        Uint32 seedY = now2 ^ ((Uint32)get_ent->id * 2654435761u);

        float directionX = (float)((seedX & 1023u) + 1u);
        float directionY = (float)((seedY & 1023u) + 1u);

        if (seedX & 2048u) {
            directionX = -directionX;
        }
        if (seedY & 4096u) {
            directionY = -directionY;
        }
        float hypotenuseLengh = sqrt(directionX * directionX + directionY * directionY);
        float invHyp = get_ent->speed / hypotenuseLengh;
        vectorX = directionX * invHyp;
        vectorY = directionY * invHyp;
        get_ent->lastTick = now2;
    }

    dummyPositionX += vectorX * delta;
    dummyPositionY += vectorY * delta;

    if (map->mapLeftLimit > dummyPositionX) {
        vectorX = -vectorX;
    }

    if (dummyPositionX > map->mapRightLimit - get_ent->dimensionX) {
        vectorX = -vectorX;
    }

    if (map->mapTopLimit > dummyPositionY) {
        vectorY = -vectorY;
    }

    if (dummyPositionY > map->mapBottomLimit - get_ent->dimensionY) {
        vectorY = -vectorY;
    }

    get_ent->positionX += vectorX * delta;
    get_ent->positionY += vectorY * delta;

    get_ent->vectorX = vectorX;
    get_ent->vectorY = vectorY;

}
