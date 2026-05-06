#include "setArmiesPosition.h"

void set_armies_in_the_battlefield(Armies *armies) {

    unsigned int battalion_size = 50;
    unsigned int number_of_battalions = 3;
    unsigned int number_of_armies = 2;
    signed int starting_point_X = 50;
    signed int starting_point_Y = 50;
    signed int ongoing_point_X = starting_point_X;
    signed int ongoing_point_Y = starting_point_Y;

    Entity *entities_ptr = armies->army->battalions->entities
    Army *army = armies->army;

    unsigned int i = 0;
    for (unsigned int j = 0; j < number_of_armies; j++) {
        Battalion *battalions = army[j].battalions;
        for (unsigned int k = 0; k < number_of_battalions; k++) {
            Entity *entities = battalions[k].entities;
            for (unsigned int l = 0; l < 5; l++) {
                for (unsigned int m = 0; m < 10; m++) {
                    entities[i].positionX = ongoing_point_X;
                    entities[i].positionY = ongoing_point_Y;
                    ongoing_point_Y += 3;
                    i++;
                }
                ongoing_point_X += 5;
                ongoing_point_Y = starting_point_Y;
            }
            ongoing_point_X = starting_point_X;
            starting_point_Y *= 2;
            ongoing_point_Y = starting_point_Y;
        }
        starting_point_X *= 10;
        ongoing_point_X = starting_point_X;
        ongoing_point_Y = 50;
        starting_point_Y = 50;
    }
}