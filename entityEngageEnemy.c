#include "entityEngageEnemy.h"

void entityFindEnemy(Entity *entities, Quadrant *quadrant, int y, int q) {
    for (int i = 0; i < y; i++) {
        if (entities[i].enemyEntityId != 0) continue;
        for (int j = 0; j < q; j++) {
            for (int k = 0; k < 10; k++) {
                if (quadrant[j].innerEntsInQuad.entitiesInsideQuadrantById[k] != 0 &&
                    quadrant[j].innerEntsInQuad.entitiesInsideQuadrantById[k] != entities[i].id) {
                    entities[i].enemyEntityId = quadrant[j].innerEntsInQuad.entitiesInsideQuadrantById[k];
                    break;
                }
            }
            if (entities[i].enemyEntityId != 0) break;
        }
    }
}


void entityEngageEnemy(Entity *entities, GameState *game, Map *map, int y) {
    for (int i = 0; i < y; i++) {
        int enemyEntityId = entities[i].enemyEntityId;
        if (enemyEntityId != 0) {
            float enemyPositionX = entities[enemyEntityId - 1].positionX;
            float enemyPositionY = entities[enemyEntityId - 1].positionY;
            
            float directionX = enemyPositionX - entities[i].positionX;
            float directionY = enemyPositionY - entities[i].positionY;


            //float dummyPositionX, dummyPositionY;

            float delta = game->delta;
            float hypo = directionX * directionX + directionY * directionY;
            // modern fast invsqrt implementation (Quake III Arena)
            float halfHypo = 0.5f * hypo;
            uint32_t w;
            memcpy(&w, &hypo, sizeof(w));
            w = 0x5f3759df - (w >> 1);
            memcpy(&hypo, &w, sizeof(hypo));
            float invHypo = 1.5f * hypo - halfHypo * hypo * hypo * hypo; // newton formula for inverse sqrt
            float scale = invHypo * entities[i].speed * delta;
            float vectorX = directionX * scale;
            float vectorY = directionY * scale;

            float dummyPositionX = entities[i].positionX + vectorX;
            float dummyPositionY = entities[i].positionY + vectorY;

            if (map->mapLeftLimit > dummyPositionX) {
                vectorX = -vectorX;
            }

            if (dummyPositionX > map->mapRightLimit - entities[i].dimensionX) {
                vectorX = -vectorX;
            }

            if (map->mapTopLimit > dummyPositionY) {
                vectorY = -vectorY;
            }

            if (dummyPositionY > map->mapBottomLimit - entities[i].dimensionY) {
                vectorY = -vectorY;
            }
            
            //entities[i].positionX += vectorX * delta;
            //entities[i].positionY += vectorY * delta;

            entities[i].vectorX = vectorX;
            entities[i].vectorY = vectorY;
        }
    }
}


