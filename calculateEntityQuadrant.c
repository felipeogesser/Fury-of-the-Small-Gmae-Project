#include "calculateEntityQuadrant.h"
#include "quadrant.h"
#include <stdbool.h>

// futuramente trocar [i] por ->

void initialCheckEntityQuadrant(GameState *game, Entity *entities, Quadrant *quadrant, int y) {
    int i;
    for (i = 0; i < y; i++) {
        int column = (int)floor(entities[i].positionX / game->quadrantSize);
        int X = column * game->quadrantSize;
        int row = (int)floor(entities[i].positionY / game->quadrantSize);
        int Y = row * game->quadrantSize;

        int indexer = row * amountX + column;
       
        if (indexer < 0 || indexer >= amountX * amountY) {
            printf("indexer = %d, column = %d, X = %d, row = %d, Y = %d\n", indexer, column, X, row, Y);
        }
        


        entities[i].currentQuadrants[0] = quadrant[indexer].id;
        //entities[i].alreadyInQuadrant |= QUAD1;

        entities[i].currentQuadrants[1] = 0;
        entities[i].currentQuadrants[2] = 0;
        entities[i].currentQuadrants[3] = 0;

        if (indexer + 1 < amountX * amountY && indexer + 1 != (row + 1) * amountX) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD2;
        } else {
            entities[i].quadrantOutOfBounds |= OOBQUAD2;
            //entities[i].alreadyInQuadrant &= ~QUAD2;
        }


        if (indexer + amountX < amountX * amountY) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD3;
        } else {
            entities[i].quadrantOutOfBounds |= OOBQUAD3;
            //entities[i].alreadyInQuadrant &= ~QUAD3;
        }

        if (indexer + amountX + 1 < amountX * amountY && indexer + amountX + 1 != (row + 2) * amountX) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD4;
        } else {
            entities[i].quadrantOutOfBounds |= OOBQUAD4;
            //entities[i].alreadyInQuadrant &= ~QUAD4;
        }



        if (!(entities[i].quadrantOutOfBounds & QUAD2) && entities[i].positionX + entities[i].dimensionX > X + game->quadrantSize) {
            quadrant[indexer + 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[1] = quadrant[indexer + 1].id;
            //entities[i].alreadyInQuadrant |= QUAD2;
        }

        if (!(entities[i].quadrantOutOfBounds & QUAD3) && entities[i].positionY + entities[i].dimensionY > Y + game->quadrantSize) {
            quadrant[indexer + amountX].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[2] = quadrant[indexer + amountX].id;
            //entities[i].alreadyInQuadrant |= QUAD3;
        }

        if (entities[i].currentQuadrants[1] != 0 &&
            entities[i].currentQuadrants[2] != 0) {
            quadrant[indexer + amountX + 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[3] = quadrant[indexer + amountX + 1].id;
            //entities[i].alreadyInQuadrant |= QUAD4;
        }
    }
}




// entities goes into indexes = to their id -1
void checkEntityQuadrant(GameState *game, Entity *entities, Quadrant *quadrant, int y) {
    int i;
    for (i = 0; i < y; i++) {
        int column = (int)floor((entities[i].positionX) / game->quadrantSize);
        int X = column * game->quadrantSize;
        int row = (int)floor((entities[i].positionY) / game->quadrantSize);
        int Y = row * game->quadrantSize;
        
        int indexer = row * amountX + column;
        
        if (indexer < 0 || indexer >= amountX * amountY) {
            printf("indexer = %d, column = %d, X = %d, row = %d, Y = %d\n", indexer, column, X, row, Y);
        }

        if (entities[i].currentQuadrants[0] != quadrant[indexer].id) {
            entities[i].currentQuadrants[0] = 0; // aqui da de ver, quando quadrant 0 mudar, zerar os outros quads
        }

        // else's clears trackers' records
        if (indexer + 1 < amountX * amountY && indexer + 1 != (row + 1) * amountX) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD2;
            // if exiting previous quadrant
            if ((entities[i].currentQuadrants[1] != 0 &&
                entities[i].currentQuadrants[1] != quadrant[indexer + 1].id) ||
                (entities[i].currentQuadrants[1] != 0 &&
                entities[i].positionX + entities[i].dimensionX < X + game->quadrantSize)) {
                quadrant[entities[i].currentQuadrants[1] - 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[1] = 0;
                //entities[i].alreadyInQuadrant &= ~QUAD2;
            }
        } else {
            if(entities[i].currentQuadrants[1] != 0) {
                quadrant[entities[i].currentQuadrants[1] - 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[1] = 0;
                entities[i].quadrantOutOfBounds |= OOBQUAD2;
                //entities[i].alreadyInQuadrant &= ~QUAD2;
            }
        }


        if (indexer + amountX < amountX * amountY) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD3;
            if ((entities[i].currentQuadrants[2] != 0 &&
                entities[i].currentQuadrants[2] != quadrant[indexer + amountX].id) ||
                (entities[i].currentQuadrants[2] != 0 &&
                entities[i].positionY + entities[i].dimensionY < Y + game->quadrantSize)) {
                quadrant[entities[i].currentQuadrants[2] - 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[2] = 0;
                //entities[i].alreadyInQuadrant &= ~QUAD3;
            }
        } else {
            if(entities[i].currentQuadrants[2] != 0) {
                quadrant[entities[i].currentQuadrants[2] - 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[2] = 0;
                entities[i].quadrantOutOfBounds |= OOBQUAD3;
                //entities[i].alreadyInQuadrant &= ~QUAD3;
            }
        }

        if (indexer + amountX + 1 < amountX * amountY && indexer + amountX + 1 != (row + 2) * amountX) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD4;
            if (entities[i].currentQuadrants[3] != 0 && entities[i].currentQuadrants[3] != quadrant[indexer + amountX + 1].id) {
                quadrant[entities[i].currentQuadrants[3] - 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[3] = 0;
                //entities[i].alreadyInQuadrant &= ~QUAD4;
            }
        } else {
            if(entities[i].currentQuadrants[3] != 0) {
                quadrant[entities[i].currentQuadrants[3] - 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[3] = 0;
                entities[i].quadrantOutOfBounds |= OOBQUAD4;
                //entities[i].alreadyInQuadrant &= ~QUAD4;
            }
        }





        // if first time entering quadrant
        if (entities[i].currentQuadrants[0] == 0) {
            quadrant[indexer].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[0] = quadrant[indexer].id;
            //entities[i].alreadyInQuadrant |= QUAD1;
            //quadrant[indexer].innerEntsInQuad.entitiesInQuadrantCount++;
        }

        
        if (entities[i].currentQuadrants[1] == 0 &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD2) &&
            entities[i].positionX + entities[i].dimensionX > X + game->quadrantSize) {
            quadrant[indexer + 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[1] = quadrant[indexer + 1].id;
            //entities[i].alreadyInQuadrant |= QUAD2;
        }

        if (entities[i].currentQuadrants[2] == 0 &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD3) &&
            entities[i].positionY + entities[i].dimensionY > Y + game->quadrantSize) {
            quadrant[indexer + amountX].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[2] = quadrant[indexer + amountX].id;
            //entities[i].alreadyInQuadrant |= QUAD3;
        }

        if (entities[i].currentQuadrants[3] == 0 &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD4) &&
            entities[i].currentQuadrants[1] != 0 &&
            entities[i].currentQuadrants[2] != 0) {
            quadrant[indexer + amountX + 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[3] = quadrant[indexer + amountX + 1].id;
            //entities[i].alreadyInQuadrant |= QUAD4;
        }

    }
}


void renderQuadrantsSetup(SDL_Renderer *ren, GameState *game, Entity *entities, Quadrant *quadrant, int y) {
    int i;
    for (i = 0; i < y; i++) {
        int column = (int)floor(entities[i].positionX / game->quadrantSize);
        int X = column * game->quadrantSize;
        int row = (int)floor(entities[i].positionY / game->quadrantSize);
        int Y = row * game->quadrantSize;

        entities[i].previousCol = column;
        entities[i].previousRow = row;
        entities[i].previousX = X;
        entities[i].previousY = Y;

        entities[i].quadrantOccupiedX += X - game->offSetX;
        entities[i].quadrantOccupiedY += Y - game->offSetY;
    }
}
// change quadrant rendering from entity rendering it to wuadrant itself be the renderer
void renderQuadrants(SDL_Renderer *ren, GameState *game, Entity *entities, Quadrant *quadrant, int y) {
    int i;
    for (i = 0; i < y; i++) {
        int column = (int)floor(entities[i].positionX / game->quadrantSize);
        int X = column * game->quadrantSize;
        int row = (int)floor(entities[i].positionY / game->quadrantSize);
        int Y = row * game->quadrantSize;

        if (entities[i].previousCol != column) {
            entities[i].quadrantOccupiedX += X - entities[i].previousX;
        }
        if (entities[i].previousRow != row) {
            entities[i].quadrantOccupiedY += Y - entities[i].previousY;
        }

        entities[i].quadrantOccupiedX -= game->vxdt + game->LX + game->KX;
        entities[i].quadrantOccupiedY -= game->vydt + game->LY + game->KY;

        entities[i].previousCol = column;
        entities[i].previousRow = row;
        entities[i].previousX = X;
        entities[i].previousY = Y;

        SDL_Rect quadrants1 = { entities[i].quadrantOccupiedX, entities[i].quadrantOccupiedY, 100, 100 };
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
        SDL_RenderFillRect(ren, &quadrants1);


        if (entities[i].currentQuadrants[1] != 0) {
            SDL_Rect quadrants2 = { entities[i].quadrantOccupiedX + 100, entities[i].quadrantOccupiedY, 100, 100 };
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
            SDL_RenderFillRect(ren, &quadrants2);
        }
        
        if (entities[i].currentQuadrants[2] != 0) {
            SDL_Rect quadrants3 = { entities[i].quadrantOccupiedX, entities[i].quadrantOccupiedY + 100, 100, 100 };
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
            SDL_RenderFillRect(ren, &quadrants3);
        }
        
        if (entities[i].currentQuadrants[3] != 0) {
            SDL_Rect quadrants4 = { entities[i].quadrantOccupiedX + 100, entities[i].quadrantOccupiedY + 100, 100, 100 };
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
            SDL_RenderFillRect(ren, &quadrants4);
        }
    }    
}