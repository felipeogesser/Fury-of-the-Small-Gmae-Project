#include "calculateEntityQuadrant.h"
#include "quadrant.h"
#include <stdbool.h>

void initialCheckEntityQuadrant(GameState *game, Entity *entities, Quadrant *quadrant, int y) {
    int i;
    for (i = 0; i < y; i++) {
        int column = abs(floor(entities[i].positionOnMapX / game->quadrantSize));
        int X = column * game->quadrantSize;
        int row = abs(floor(entities[i].positionOnMapY / game->quadrantSize));
        int Y = row * game->quadrantSize;

        int indexer = row * amountX + column;
       
        if (indexer < 0 || indexer >= amountX * amountY) {
            quadrant[-1].id = 0;
        }
        

        if (indexer + 1 < amountX * amountY && indexer + 1 != (row + 1) * amountX) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD2;
        } else {
            entities[i].currentQuadrants[1] = 0;
            entities[i].quadrantOutOfBounds |= OOBQUAD2;
            entities[i].alreadyInQuadrant &= ~QUAD2;
        }


        if (indexer + amountX < amountX * amountY) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD3;
        } else {
            entities[i].currentQuadrants[2] = 0;
            entities[i].quadrantOutOfBounds |= OOBQUAD3;
            entities[i].alreadyInQuadrant &= ~QUAD3;
        }

        if (indexer + amountX + 1 < amountX * amountY && indexer + amountX + 1 != (row + 2) * amountX) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD4;
        } else {
            entities[i].currentQuadrants[3] = 0;
            entities[i].quadrantOutOfBounds |= OOBQUAD4;
            entities[i].alreadyInQuadrant &= ~QUAD4;
        }

        entities[i].currentQuadrants[0] = quadrant[indexer].id;
        entities[i].alreadyInQuadrant |= QUAD1;

        if (!(entities[i].quadrantOutOfBounds & OOBQUAD2) &&
            entities[i].positionOnMapX + entities[i].dimensionX > X + game->quadrantSize) {
            int auxIndex2;
            auxIndex2 = entities[i].currentQuadrants[1] - 1;
            quadrant[auxIndex2].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
            quadrant[indexer + 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[1] = quadrant[indexer + 1].id;
            entities[i].alreadyInQuadrant |= QUAD2;
        }

        if (!(entities[i].quadrantOutOfBounds & OOBQUAD3) &&
            entities[i].positionOnMapY + entities[i].dimensionY > Y + game->quadrantSize) {
            int auxIndex3;
            auxIndex3 = entities[i].currentQuadrants[2] - 1;
            quadrant[auxIndex3].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
            quadrant[indexer + amountX].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[2] = quadrant[indexer + amountX].id;
            entities[i].alreadyInQuadrant |= QUAD3;
        }

        if (!(entities[i].quadrantOutOfBounds & OOBQUAD4) &&
            entities[i].alreadyInQuadrant & QUAD2 &&
            entities[i].alreadyInQuadrant & QUAD3) {
            int auxIndex4;
            auxIndex4 = entities[i].currentQuadrants[3] - 1;
            quadrant[auxIndex4].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
            quadrant[indexer + amountX + 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[3] = quadrant[indexer + amountX + 1].id;
            entities[i].alreadyInQuadrant |= QUAD4;
        }
    }
}




// entities goes into indexes = to their id -1
void checkEntityQuadrant(GameState *game, Entity *entities, Quadrant *quadrant, int y) {
    int i;
    for (i = 0; i < y; i++) {
        int column = abs(floor((entities[i].positionOnMapX) / game->quadrantSize));
        int X = column * game->quadrantSize;
        int row = abs(floor((entities[i].positionOnMapY) / game->quadrantSize));
        int Y = row * game->quadrantSize;
        
        int indexer = row * amountX + column;

        if (entities[i].currentQuadrants[0] == quadrant[indexer].id) {
            entities[i].alreadyInQuadrant |= QUAD1;
        } else {
            entities[i].alreadyInQuadrant &= ~QUAD1;
        }
        
        if (indexer + 1 < amountX * amountY && indexer + 1 != (row + 1) * amountX) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD2;
            if (entities[i].currentQuadrants[1] == quadrant[indexer + 1].id) {
                entities[i].alreadyInQuadrant |= QUAD2;
            }
        } else {
            entities[i].currentQuadrants[1] = 0;
            entities[i].quadrantOutOfBounds |= OOBQUAD2;
            entities[i].alreadyInQuadrant &= ~QUAD2;
        }


        if (indexer + amountX < amountX * amountY) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD3;
            if (entities[i].currentQuadrants[2] == quadrant[indexer + amountX].id) {
                entities[i].alreadyInQuadrant |= QUAD3;
            }
        } else {
            entities[i].currentQuadrants[2] = 0;
            entities[i].quadrantOutOfBounds |= OOBQUAD3;
            entities[i].alreadyInQuadrant &= ~QUAD3;
        }

        if (indexer + amountX + 1 < amountX * amountY && indexer + amountX + 1 != (row + 2) * amountX) {
            entities[i].quadrantOutOfBounds &= ~OOBQUAD4;
            if (entities[i].currentQuadrants[3] == quadrant[indexer + amountX + 1].id) {
                entities[i].alreadyInQuadrant |= QUAD4;
            } 
        } else {
            entities[i].currentQuadrants[3] = 0;
            entities[i].quadrantOutOfBounds |= OOBQUAD4;
            entities[i].alreadyInQuadrant &= ~QUAD4;
        }





        // if first time entering quadrant
        if (!(entities[i].alreadyInQuadrant & QUAD1)) {
            // clean previous quadrant
            int auxIndex1;
            auxIndex1 = entities[i].currentQuadrants[0] - 1;
            quadrant[auxIndex1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;

            quadrant[indexer].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[0] = quadrant[indexer].id;
            entities[i].alreadyInQuadrant |= QUAD1;
            //quadrant[indexer].innerEntsInQuad.entitiesInQuadrantCount++;
        }

        
        if (!(entities[i].alreadyInQuadrant & QUAD2) &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD2) &&
            entities[i].positionOnMapX + entities[i].dimensionX > X + game->quadrantSize) {
            int auxIndex2;
            auxIndex2 = entities[i].currentQuadrants[1] - 1;
            quadrant[auxIndex2].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
            quadrant[indexer + 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[1] = quadrant[indexer + 1].id;
            entities[i].alreadyInQuadrant |= QUAD2;
        } else {
            entities[i].alreadyInQuadrant &= ~QUAD2;
        }

        if (!(entities[i].alreadyInQuadrant & QUAD3) &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD3) &&
            entities[i].positionOnMapY + entities[i].dimensionY > Y + game->quadrantSize) {
            int auxIndex3;
            auxIndex3 = entities[i].currentQuadrants[2] - 1;
            quadrant[auxIndex3].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
            quadrant[indexer + amountX].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[2] = quadrant[indexer + amountX].id;
            entities[i].alreadyInQuadrant |= QUAD3;
        } else {
            entities[i].alreadyInQuadrant &= ~QUAD3;
        }

        if (!(entities[i].alreadyInQuadrant & QUAD4) &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD4) &&
            entities[i].alreadyInQuadrant & QUAD2 &&
            entities[i].alreadyInQuadrant & QUAD3) {
            int auxIndex4;
            auxIndex4 = entities[i].currentQuadrants[3] - 1;
            quadrant[auxIndex4].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = 0;
            quadrant[indexer + amountX + 1].innerEntsInQuad.entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[3] = quadrant[indexer + amountX + 1].id;
            entities[i].alreadyInQuadrant |= QUAD4;
        } else {
            entities[i].alreadyInQuadrant &= ~QUAD4;
        }

    }
}


void renderQuadrantsFirstTime(SDL_Renderer *ren, GameState *game, Entity *entities, Quadrant *quadrant, int y) {
    int i;
    for (i = 0; i < y; i++) {
        int column = floor(entities[i].positionOnMapX / game->quadrantSize);
        int X = column * game->quadrantSize;
        int row = floor(entities[i].positionOnMapY / game->quadrantSize);
        int Y = row * game->quadrantSize;

        entities[i].previousCol = column;
        entities[i].previousRow = row;
        entities[i].previousX = X;
        entities[i].previousY = Y;

        entities[i].positionOnScreenX += X - game->ZX - game->vxdt;
        entities[i].positionOnScreenY += Y - game->ZY - game->vydt;

        SDL_Rect quadrants = { entities[i].positionOnScreenX, entities[i].positionOnScreenY, 200, 200 };
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
        SDL_RenderFillRect(ren, &quadrants);
    }
}

void renderQuadrants(SDL_Renderer *ren, GameState *game, Entity *entities, Quadrant *quadrant, int y) {
    int i;
    for (i = 0; i < y; i++) {
        int column = floor(entities[i].positionOnMapX / game->quadrantSize);
        int X = column * game->quadrantSize;
        int row = floor(entities[i].positionOnMapY / game->quadrantSize);
        int Y = row * game->quadrantSize;

        if (entities[i].previousCol != column) {
            entities[i].positionOnScreenX += X - entities[i].previousX;
        }
        if (entities[i].previousRow != row) {
            entities[i].positionOnScreenY += Y - entities[i].previousY;
        }

        entities[i].positionOnScreenX -= game->vxdt + game->LX + game->KX;
        entities[i].positionOnScreenY -= game->vydt + game->LY + game->KY;

        entities[i].previousCol = column;
        entities[i].previousRow = row;
        entities[i].previousX = X;
        entities[i].previousY = Y;

        SDL_Rect quadrants2 = { entities[i].positionOnScreenX, entities[i].positionOnScreenY, 100, 100 };
        SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
        SDL_RenderFillRect(ren, &quadrants2);


        if (entities[i].alreadyInQuadrant & QUAD2) {
            SDL_Rect quadrants2 = { entities[i].positionOnScreenX + 100, entities[i].positionOnScreenY, 100, 100 };
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
            SDL_RenderFillRect(ren, &quadrants2);
        }
        
        if (entities[i].alreadyInQuadrant & QUAD3) {
            SDL_Rect quadrants3 = { entities[i].positionOnScreenX, entities[i].positionOnScreenY + 100, 100, 100 };
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
            SDL_RenderFillRect(ren, &quadrants3);
        }
        
        if (entities[i].alreadyInQuadrant & QUAD4) {
            SDL_Rect quadrants4 = { entities[i].positionOnScreenX + 100, entities[i].positionOnScreenY + 100, 100, 100 };
            SDL_SetRenderDrawColor(ren, 0, 255, 0, 100);
            SDL_RenderFillRect(ren, &quadrants4);
        }
    }    
}