#include "playerObjectCollision.h"

void isPlayerCollidingObject(Player *player, float (*hitBoxObject)[4][2], GameState *game, int x) {
    
    float hitBoxPlayer[4][2] = {
        {player->playerPositionX, player->playerPositionY},
        {player->playerPositionX + player->playerDimensionX, player->playerPositionY},
        {player->playerPositionX, player->playerPositionY + player->playerDimensionY},
        {player->playerPositionX + player->playerDimensionX, player->playerPositionY + player->playerDimensionY}
    }

    if (game->vx > 0 && game->vy > 0) {
        for (int i = 0; i < x; i++) {
            // Object *obj = get_object(object_id[i]);
            if ((hitBoxPlayer[3][1] <  hitBoxObject[i][0][1] -game->vydt -1 ||
                hitBoxPlayer[1][1] >  hitBoxObject[i][2][1] -game->vydt +1) &&
                (hitBoxPlayer[3][0] <  hitBoxObject[i][0][0] -game->vxdt -1 ||
                hitBoxPlayer[2][0] >  hitBoxObject[i][1][0] -game->vxdt +1)) {
            } else
            if ((hitBoxPlayer[1][0] >= hitBoxObject[i][0][0] -game->vxdt -1 &&
                hitBoxPlayer[1][0] <  hitBoxObject[i][0][0] + 1) &&
                (hitBoxPlayer[2][1] >= hitBoxObject[i][0][1] -game->vydt -1 &&
                hitBoxPlayer[2][1] <  hitBoxObject[i][0][1] + 1)) {
                // obj->positionY = hitBoxPlayer[2][1] - 2;
                //hitBoxObject[i][0][1] -= 3;
                game->vxdt = 0;
            }
        }
    }

    if (game->vx > 0 && game->vy < 0) {
        for (int i = 0; i < x; i++) {
            //  Object *obj = get_object(object_id[i]);
            if ((hitBoxPlayer[3][1] <  hitBoxObject[i][0][1] -game->vydt -1 ||
                hitBoxPlayer[1][1] >  hitBoxObject[i][2][1] -game->vydt) +1 &&
                (hitBoxPlayer[1][0] <  hitBoxObject[i][2][0] -game->vxdt -1 ||
                hitBoxPlayer[0][0] >  hitBoxObject[i][3][0] -game->vxdt +1)) {
            } else
            if ((hitBoxPlayer[1][0] >= hitBoxObject[i][0][0] -game->vxdt -1 &&
                hitBoxPlayer[1][0] <  hitBoxObject[i][0][0] + 1) &&
                (hitBoxPlayer[0][1] <= hitBoxObject[i][2][1] -game->vydt +1 &&
                hitBoxPlayer[0][1] >  hitBoxObject[i][2][1] - 1)) {
                //obj->positionY = hitBoxPlayer[1][1] - obj->positionY + 2;
                //hitBoxObject[i][2][1] += 3;
                game->vxdt = 0;
            }
        }
    }

    if (game->vx < 0 && game->vy > 0) {
        for (int i = 0; i < x; i++) {
            //  Object *obj = get_object(object_id[i]);
            if ((hitBoxPlayer[2][1] <  hitBoxObject[i][1][1] -game->vydt -1 ||
                hitBoxPlayer[0][1] >  hitBoxObject[i][3][1] -game->vydt +1) &&
                (hitBoxPlayer[3][0] <  hitBoxObject[i][0][0] -game->vxdt -1 ||
                hitBoxPlayer[2][0] >  hitBoxObject[i][1][0] -game->vxdt +1)) {
            } else
            if ((hitBoxPlayer[0][0] <= hitBoxObject[i][1][0] -game->vxdt +1 &&
                hitBoxPlayer[0][0] >  hitBoxObject[i][1][0] - 1) &&
                (hitBoxPlayer[2][1] >= hitBoxObject[i][0][1] -game->vydt -1 &&
                hitBoxPlayer[2][1] <  hitBoxObject[i][0][1] + 1)) {
                // obj->positionY = hitBoxPlayer[2][1] - 2;
                //hitBoxObject[i][1][1] -= 3;
                game->vxdt = 0;
            }
        }
    }

    if (game->vx < 0 && game->vy < 0) {
        for (int i = 0; i < x; i++) {
            //   Object *obj = get_object(object_id[i]);
            if ((hitBoxPlayer[2][1] <  hitBoxObject[i][1][1] -game->vydt -1 ||
                hitBoxPlayer[0][1] >  hitBoxObject[i][3][1] -game->vydt +1) &&
                (hitBoxPlayer[1][0] <  hitBoxObject[i][2][0] -game->vxdt -1 ||
                hitBoxPlayer[0][0] >  hitBoxObject[i][3][0] -game->vxdt +1)) {
            } else
            if ((hitBoxPlayer[0][0] <= hitBoxObject[i][1][0] -game->vxdt +1 &&
                hitBoxPlayer[0][0] >  hitBoxObject[i][1][0] - 1) &&
                (hitBoxPlayer[0][1] <= hitBoxObject[i][2][1] -game->vydt +1 &&
                hitBoxPlayer[0][1] >  hitBoxObject[i][2][1] - 1)) {
                //obj->positionY = hitBoxPlayer[0][1] - obj->positionY + 2;
                //hitBoxObject[i][3][1] += 3;
                game->vxdt = 0;
            }
        }
    }


    if (game->vx > 0) {
        for (int i = 0; i < x; i++) {
            //   Object *obj = get_object(object_id[i]);
            if (hitBoxPlayer[3][1] <  hitBoxObject[i][0][1] -game->vydt -1 ||
                hitBoxPlayer[1][1] >  hitBoxObject[i][2][1] -game->vydt +1) {
            } else
            if (hitBoxPlayer[1][0] >= hitBoxObject[i][0][0] -game->vxdt -1 &&
                hitBoxPlayer[1][0] <  hitBoxObject[i][0][0] + 1) {
                game->KX = - hitBoxPlayer[1][0] + hitBoxObject[i][0][0];
                game->vxdt = 0;
                /*if (game->KX < 1) {
                    game->KX = 0;
                }*/
            }     
        }       
    }

    if (game->vx < 0) {
        for (int i = 0; i < x; i++) {
            //  Object *obj = get_object(object_id[i]);
            if (hitBoxPlayer[2][1] <  hitBoxObject[i][1][1] -game->vydt -1 ||
                hitBoxPlayer[0][1] >  hitBoxObject[i][3][1] -game->vydt +1) {
            } else
            if (hitBoxPlayer[0][0] <= hitBoxObject[i][1][0] -game->vxdt +1 &&
                hitBoxPlayer[0][0] >  hitBoxObject[i][1][0] - 1) {
                //   obj->positionX = hitBoxPlayer[0][0] - obj->dimensionX - 1;
                // MapLeftLimit += hitBoxPlayer[0][0] - hitBoxObject[i][1][0] - 1;
                game->KX = - hitBoxPlayer[0][0] + hitBoxObject[i][1][0];
                game->vxdt = 0;
                /*if (game->KX > -1) {
                    game->KX = 0;
                }*/
            }
        }
    }

    if (game->vy > 0) {
        for (int i = 0; i < x; i++) {
            // Object *obj = get_object(object_id[i]);
            if (hitBoxPlayer[3][0] <  hitBoxObject[i][0][0] -game->vxdt -1 ||
                hitBoxPlayer[2][0] >  hitBoxObject[i][1][0] -game->vxdt +1) {
            } else
            if (hitBoxPlayer[2][1] >= hitBoxObject[i][0][1] -game->vydt -1 &&
                hitBoxPlayer[2][1] <  hitBoxObject[i][0][1] + 1) {
                //obj->positionY = hitBoxPlayer[2][1] + 1;
                //MapTopLimit += hitBoxPlayer[2][1] - hitBoxObject[i][0][1] + 1;
                game->KY = - hitBoxPlayer[2][1] + hitBoxObject[i][0][1];
                game->vydt = 0;
                /*if (game->KY < 1) {
                    game->KY = 0;
                }*/
            }
        }
    }

    if (game->vy < 0) {
        for (int i = 0; i < x; i++) {
            //  Object *obj = get_object(object_id[i]);
            if (hitBoxPlayer[1][0] <  hitBoxObject[i][2][0] -game->vxdt -1 ||
                hitBoxPlayer[0][0] >  hitBoxObject[i][3][0] -game->vxdt +1) {
            } else
            if (hitBoxPlayer[0][1] <= hitBoxObject[i][2][1] -game->vydt +1 &&
                hitBoxPlayer[0][1] >  hitBoxObject[i][2][1] - 1) {
                //obj->positionY = hitBoxPlayer[1][1] - obj->dimensionY - 1;
                // MapTopLimit += hitBoxPlayer[0][1] - hitBoxObject[i][2][1] - 1;
                game->KY = - hitBoxPlayer[0][1] + hitBoxObject[i][2][1];
                game->vydt = 0;
                /*if (game->KY > -1) {
                    game->KY = 0;
                }*/
            }
        }
    }
}
