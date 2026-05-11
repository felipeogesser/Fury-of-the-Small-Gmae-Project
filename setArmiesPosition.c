#include "setArmiesPosition.h"
#include "battalion.h"
#include "entities.h"
#include "stdio.h"

void set_armies_in_the_battlefield(Armies *armies) {

    unsigned int battalion_size = 50;
    unsigned int number_of_battalions = 3;
    unsigned int number_of_armies = 2;
    signed int starting_point_X = 50;
    signed int starting_point_Y = 50;
    signed int ongoing_point_X = starting_point_X;
    signed int ongoing_point_Y = starting_point_Y;

    Army *army = armies->army;

    unsigned int i = 0;
    for (unsigned int j = 0; j < number_of_armies; j++) {

        Battalion *battalions = army[j].battalions;

        for (unsigned int k = 0; k < number_of_battalions; k++) {
            // simple color randomizer
            unsigned int x = (k + 1) * 2654435761u;

            unsigned char r = (x >>  0) & 255u;
            unsigned char g = (x >>  8) & 255u;
            unsigned char b = (x >> 16) & 255u;

            battalions[k].R_Color = r;
            battalions[k].G_Color = g;
            battalions[k].B_Color = b;
            battalions[k].Alpha = (unsigned char)255;
            Entity *entities = battalions[k].entities;

            for (unsigned int l = 0; l < 5; l++) {

                for (unsigned int m = 0; m < 10; m++) {

                    create_entities(entities, ongoing_point_X, ongoing_point_Y, i);
                    ongoing_point_Y += 6;
                    i++;
                }

                ongoing_point_X += 10;
                ongoing_point_Y = starting_point_Y;
            }

            ongoing_point_X = starting_point_X;
            starting_point_Y += 70;
            ongoing_point_Y = starting_point_Y;
            i = 0;
        }

        starting_point_X *= 10;
        ongoing_point_X = starting_point_X;
        ongoing_point_Y = 50;
        starting_point_Y = 50;
    }

    unsigned int entities_created_count = (unsigned int)(battalion_size * number_of_battalions * number_of_armies);

    Entity *entities = army->battalions->entities;

    for (i = 0; i < entities_created_count; i++) {
        entities[i].id = i + 1;
    }

} //talvez da pra remover Entity *entities = battalions[k].entities;
// e calulcar direto so entities do i = 0 ate o final, sem precisar resetar o i


/*void set_armies_in_the_battlefield(Armies *armies) {

    unsigned int battalion_size = 50;
    unsigned int number_of_battalions = 3;
    unsigned int number_of_armies = 2;
    signed int starting_point_X = 50;
    signed int starting_point_Y = 50;
    signed int ongoing_point_X = starting_point_X;
    signed int ongoing_point_Y = starting_point_Y;

    Army *army = armies->army;

    unsigned int i = 0;
    for (unsigned int j = 0; j < number_of_armies; j++) {
        Battalion *battalions = army[j].battalions;
        for (unsigned int k = 0; k < number_of_battalions; k++) {
            Entity *entities = battalions[k].entities;
            for (unsigned int l = 0; l < 5; l++) {
                for (unsigned int m = 0; m < 10; m++) {
                    create_entities(entities, ongoing_point_X, ongoing_point_Y, i);
                    ongoing_point_Y += 3;
                    i++;
                }
                ongoing_point_X += 5;
                ongoing_point_Y = starting_point_Y;
            }
            ongoing_point_X = starting_point_X;
            starting_point_Y *= 2;
            ongoing_point_Y = starting_point_Y;
            i = 0;
        }
        starting_point_X *= 10;
        ongoing_point_X = starting_point_X;
        ongoing_point_Y = 50;
        starting_point_Y = 50;
    }

    unsigned int entities_created_count = (unsigned int)(battalion_size * number_of_battalions * number_of_armies);

    Entity *entities = army->battalions->entities;

    for (i = 0; i < entities_created_count; i++) {
        entities[i].id = i + 1;
    }

} */