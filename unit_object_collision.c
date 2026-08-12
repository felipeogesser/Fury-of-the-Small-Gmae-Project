#include "unit_object_collision.h"

void unit_object_collision(Unit *unit, Quadrant *quadrant, float (*hitBoxObject)[4][2], GameState *game, int x, int y) {
    for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            unsigned char COLLISION = 0;
            float positionX = unit[j].positionX;
            float positionY = unit[j].positionY;
            float vectorX = unit[j].vectorX;
            float vectorY = unit[j].vectorY;
            positionX += vectorX;
            positionY += vectorY;
            float dimensionX = unit[j].dimensionX;
            float dimensionY = unit[j].dimensionY;
            float posDimX = positionX + dimensionX;
            float posDimY = positionY + dimensionY;

            if (unit[j].vectorY > 0 &&
                unit[j].vectorY + unit[j].positionY + unit[j].dimensionY >=
                hitBoxObject[i][0][0] &&
                hitBoxObject[i][0][0] > unit[j].positionY + unit[j].dimensionY) {
                    unit[j].vectorY *= -1.0f;
            }

            if (positionX < hitBoxObject[i][1][0] &&
                posDimY > hitBoxObject[i][1][1]) {
                    COLLISION |= CORNER2;
                    COLLISION |= (posDimY - hitBoxObject[i][1][1] >
                    hitBoxObject[i][1][0] - positionX) ? FLIPVECX : FLIPVECY;
            }
            
            if (posDimX > hitBoxObject[i][2][0] &&
                positionY < hitBoxObject[i][2][1]) {
                    COLLISION |= CORNER3;
                    COLLISION |= (posDimX - hitBoxObject[i][2][0] <
                    hitBoxObject[i][2][1] - positionY) ? FLIPVECX2 : FLIPVECY2;
            }

            float gx = 1.0f;
            float gy = 1.0f;

            if ((COLLISION & CORNER2) && (COLLISION & CORNER3)) {
                if (posDimX - vectorX > hitBoxObject[i][2][0] && // se isso estiver dentro dos if anterior, ou seja, teve colisao, a expressao da pra reduzir e fazer flipx : flipY ao inves de mais uma condicao apos :
                    positionY - vectorY < hitBoxObject[i][2][1]) {
                    (COLLISION & FLIPVECX) ? gx *= -1.0f : (COLLISION & FLIPVECY) ? gy *= -1.0f : (void)0;
                } else {
                    (COLLISION & FLIPVECX2) ? gx *= -1.0f : (COLLISION & FLIPVECY2) ? gy *= -1.0f : (void)0;
                }
            }
            unit[j].vectorX *= gx;
            unit[j].vectorY *= gy;
        }
    }


}