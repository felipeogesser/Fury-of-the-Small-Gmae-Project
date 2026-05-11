#include "teamsPositionAssembler.h"
//#include "windowSettings.h"
//#include "maps.h"
#include "entities.h"

float px = 0.0f;
float py = 0.0f;
int indexxx = 0;
int k, j;
//float shiftX = 800/ 20;
//float shiftY = 600 / 20;
void set_team_position(Entity *entities) {
   // for (indexxx = 0; indexxx < MAX_ENTITIES; indexxx++) {
    //    if (entities[indexxx].id != indexxx + 1) break;
    //    if (entities[indexxx].team != "blue") break;
        for (k = 0; k < 10; k++) {
            py += 50.0f;
            px = 50.0f;
            for (j = 0; j < 5; j++) {
                px += 50.0f;
                entities[indexxx].positionX = px;
                entities[indexxx].positionY = py;
                indexxx++;
            }
        } 
   // }
    
    px = 800 - 20.0f;
    py = 0.0f;
    //for (indexxx = 0; indexxx < MAX_ENTITIES; indexxx++) {
   //     if (entities[indexxx].id != indexxx + 1) break;
        // if (entities[indexxx].team != "red") break;
        for (k = 0; k < 10; k++) {
            py += 50.0f;
            px = 800 - 20.0f;
            for (j = 0; j < 5; j++) {
                px -= 50.0f;
                entities[indexxx].positionX = px;
                entities[indexxx].positionY = py;
                indexxx++;
            }
        } 
   // }


}
