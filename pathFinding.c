#include "pathFinding.h"
#include <stdio.h>
#include <stdbool.h>
#include <math.h>

// here we calculate the intersection of the line between two entities and
// the diagonal lines inside an object of four sides in order to validate
// if the path being taken by the entity collides with an object or not
void entityToEntityPathFinding(GameState *game, Entity *entities, Object *object, int x, int y) {
    for












}






































































/*
        float vectorToEntityX = entities[j].vectorX;
        float vectorToEntityY = entities[j].vectorY;
        int enemyEntityId = entities[j].enemyEntityId;

        if (enemyEntityId == 0) {
            printf("no enemy");
        }
        if (enemyEntityId < 0) {
            printf("enemy id is negative");
        }
        
        float P1x = entities[j].positionX;
        float P1y = entities[j].positionY;

        float P2x = entities[enemyEntityId - 1].positionX;
        float P2y = entities[enemyEntityId - 1].positionY;

        float L1x = P1x + (P2x - P1x) * t;
        float L1y = P1y + (P2y - P1y) * t;

        float P3x = object[i].postionX;
        float P3y = object[i].postionY;

        float P4x =  object[i].postionX + object[i].dimensionX;
        float P4y =  object[i].postionY + object[i].dimensionY;

        float L2x = P3x + (P4x - P3x) * u;
        float L2y = P3y + (P4y - P3y) * u;

        t*(P2x - P1x) - u*(P4x - P3x) = P3x - P1x
        t*(P2y - P1y) - u*(P4y - P3y) = P3y - P1y

        t = cross((x3 - x1, y3 - y1), S) / cross(R, S)
        u = cross((x3 - x1, y3 - y1), R) / cross(R, S)

        R = (x2 - x1, y2 - y1)
        S = (x4 - x3, y4 - y3)

        cross(R, S) =
        (x2 - x1)*(y4 - y3) - (y2 - y1)*(x4 - x3)

        t = cross((x3 - x1, y3 - y1), S) / cross(R, S)
        u = cross((x3 - x1, y3 - y1), R) / cross(R, S)

        den = cross(R, S)
        den = (P2x - P1x)*(P4y - P3y) - (P2y - P1y)*(P4x - P3x);
       // if den is 0 lines are parallel or collinear

        t = ((P3x - P1x)*(P4y - P3y) - (P3y - P1y)*(P4x - P3x)) / den
        u = ((P3x - P1x)*(P2y - P1y) - (P3y - P1y)*(P2x - P1x)) / den



        

        /*if (t >= 0 && t <= 1 &&
            u >= 0 && u <= 1) {
            // segments intersect
        }*/



    /*for (int i = 0; i < x; i++) {
        for (int j = 0; j < y; j++) {
            if (!entities[j].pathFound) {
                int enemyEntityId = entities[j].enemyEntityId;
                bool intersect = false;
                if (enemyEntityId == 0) {
                    printf("no enemy");
                }
                if (enemyEntityId < 0) {
                    printf("enemy id is negative");
                }
                
                // entity-entity line
                float P1x = entities[j].positionX;
                float P1y = entities[j].positionY;
                float P2x = entities[enemyEntityId - 1].positionX;
                float P2y = entities[enemyEntityId - 1].positionY;
                // first diagonal line
                float P3x = object[i].positionX;
                float P3y = object[i].positionY;
                float P4x = object[i].positionX + object[i].dimensionX;
                float P4y = object[i].positionY + object[i].dimensionY;
                // second diagonal line
                float P5x = object[i].positionX + object[i].dimensionX;
                float P5y = object[i].positionY;
                float P6x = object[i].positionX;
                float P6y = object[i].positionY + object[i].dimensionY;

                float P21x = P2x - P1x;
                float P21y = P2y - P1y;
                float P31x = P3x - P1x;
                float P31y = P3y - P1y;
                float P43x = P4x - P3x;
                float P43y = P4y - P3y;
                float P51x = P5x - P1x;
                float P51y = P5y - P1y;
                float P65x = P6x - P5x;
                float P65y = P6y - P5y;


                // if den is 0 lines are parallel or collinear

                float den1 = P21x * P43y - P21y * P43x;
                float den2 = P21x * P65y - P21y * P65x;

                if (__builtin_expect(den1 != 0 && den2 != 0, 1)) {

                    float t1 = (P31x * P43y - P31y * P43x) / den1;
                    float u1 = (P31x * P21y - P31y * P21x) / den1;

                    float t2 = (P51x * P65y - P51y * P65x) / den2;
                    float u2 = (P51x * P21y - P51y * P21x) / den2;

                    intersect = ((t1 >= 0 && t1 <= 1 && u1 >= 0 && u1 <= 1) ||
                                (t2 >= 0 && t2 <= 1 && u2 >= 0 && u2 <= 1)) ? true : false;

                } else {
                    if (den1) {
                        float t1 = (P31x * P43y - P31y * P43x) / den1;
                        float u1 = (P31x * P21y - P31y * P21x) / den1;

                        intersect = (t1 >= 0 && t1 <= 1 && u1 >= 0 && u1 <= 1) ? true : false;
                    }

                    if (den2) {
                        float t2 = (P51x * P65y - P51y * P65x) / den2;
                        float u2 = (P51x * P21y - P51y * P21x) / den2;

                        intersect = (t2 >= 0 && t2 <= 1 && u2 >= 0 && u2 <= 1) ? true : false;
                    }
                }

                //int objectCollidedId = 0;

                if (!intersect) {
                    objectCollidedId = object[i].id;

                    int quadrant1 = object[i].quadrant[0]
                    int quadrant2 = object[i].quadrant[1]
                    int quadrant3 = object[i].quadrant[2]
                    
                    // first diagonal
                    //P31x + P31y + (P2x - P3x) + (P2y - P3y);

                    //(P4x - P1x + P4y - P1y) + (P2x - P4x) + (P2y - P4y)
                    
                    // first path
                    // first hypo entity to object
                    float val1 = P31x * P31x;
                    float val2 = P31y * P31y;
                    
                    // second hypo object to enemy
                    float val3 = (P2x - P3x) * (P2x - P3x);
                    float val4 = (P2y - P3y) * (P2y - P3y);

                    float hypo12 = sqrt(val1 + val2);
                    float hypo34 = sqrt(val3 + val4);
                    float hypoTotal1 = hypo12 + hypo34;
                    
                    // second path
                    // third hypo entity to object
                    float val5 = (P4x - P1x) * (P4x - P1x);
                    float val6 = (P4y - P1y) * (P4y - P1y);
                    
                    //forth hypo object to enemy
                    float val7 = (P2x - P4x) * (P2x - P4x);
                    float val8 = (P2y - P4y) * (P2y - P4y);

                    float hypo56 = sqrt(val5 + val6);
                    float hypo78 = sqrt(val7 + val8);
                    float hypoTotal2 = hypo56 + hypo78;

                    if (hypoTotal1 > hypoTotal2) {
                        entities[j].constMovX = P31x;
                        entities[j].constMovY = P31y;
                        entities[j].vectorX = P31x * entities[j].speed / hypo12;
                        entities[j].vectorY = P31y * entities[j].speed / hypo12;
                        entities[j].pathFound = true;
                    } else {
                        entities[j].constMovX = (P4x - P1x);
                        entities[j].constMovY = (P4y - P1y);
                        entities[j].vectorX = (P4x - P1x) * entities[j].speed / hypo56;
                        entities[j].vectorY = (P4y - P1y) * entities[j].speed / hypo56;
                        entities[j].pathFound = true;
                    }

                } else {
                    entities[j].vectorX = 
                    entities[j].vectorY = 
                }
                // second diagonal

            }

        }
    }*/
