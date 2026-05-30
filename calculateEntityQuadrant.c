#include "calculateEntityQuadrant.h"
#include "quadrant.h"
#include "stdio.h"
#include "engine.h"
#include "loadArmies.h"
// futuramente trocar [i] por ->

void initialCheckEntityQuadrant(Armies *armies, GameState *game, Grids *grids) {

    Entity *entities = armies->army->battalions->entities;
    signed int amountX = game->amountX;
    signed int amountY = game->amountY;
    bigQuadrant *quadrant = grids->GLLoD->bigQuad;

    for (unsigned int i = 0; i < game->entities_created_count; i++) {
        
        int column = (int)floor(entities[i].positionX / game->low_LOD_quadrant_size);
        int X = column * (signed int)game->low_LOD_quadrant_size;
        int row = (int)floor(entities[i].positionY / game->low_LOD_quadrant_size);
        int Y = row * game->low_LOD_quadrant_size;
 
        signed int indexer = row * amountX + column;

        if (indexer < 0 || indexer >= amountX * amountY) {
            printf("indexer = %d, column = %d, X = %d, row = %d, Y = %d\n", indexer, column, X, row, Y);
        }
        
        quadrant[indexer].entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
        entities[i].currentQuadrants[0] = quadrant[indexer].id;
        //entities[i].alreadyInQuadrant |= QUAD1;

        entities[i].currentQuadrants[1] = 0;
        entities[i].currentQuadrants[2] = 0;
        entities[i].currentQuadrants[3] = 0;

        if (indexer + 1 < amountX * amountY && indexer + 1 != (row + 1) * amountX) {
            entities[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD2);
        } else {
            entities[i].quadrantOutOfBounds |= OOBQUAD2;
            //entities[i].alreadyInQuadrant &= ~QUAD2;
        }

        if (indexer + amountX < amountX * amountY) {
            entities[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD3);
        } else {
            entities[i].quadrantOutOfBounds |= OOBQUAD3;
            //entities[i].alreadyInQuadrant &= ~QUAD3;
        }
                                                            //indexer + 1 != (row + 1) * amountX
        if (indexer + amountX + 1 < amountX * amountY && indexer + amountX + 1 != (row + 2) * amountX) {
            entities[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD4);
        } else {
            entities[i].quadrantOutOfBounds |= OOBQUAD4;
            //entities[i].alreadyInQuadrant &= ~QUAD4;
        }

        if (!(entities[i].quadrantOutOfBounds & QUAD2) && entities[i].positionX + entities[i].dimensionX > X + game->low_LOD_quadrant_size) {
            quadrant[indexer + 1].entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[1] = quadrant[indexer + 1].id;
            //entities[i].alreadyInQuadrant |= QUAD2;
        }

        if (!(entities[i].quadrantOutOfBounds & QUAD3) && entities[i].positionY + entities[i].dimensionY > Y + game->low_LOD_quadrant_size) {
            quadrant[indexer + amountX].entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[2] = quadrant[indexer + amountX].id;
            //entities[i].alreadyInQuadrant |= QUAD3;
        }

        if (entities[i].currentQuadrants[1] != 0 &&
            entities[i].currentQuadrants[2] != 0) {
            quadrant[indexer + amountX + 1].entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[3] = quadrant[indexer + amountX + 1].id;
            //entities[i].alreadyInQuadrant |= QUAD4;
        }
    }
}




// entities goes into indexes = to their id -1
void check_entity_quadrant(Armies *armies, GameState *game, Grids *grids) {

    Entity *entities = armies->army->battalions->entities;
    signed int amountX = game->amountX;
    signed int amountY = game->amountY;
    bigQuadrant *quadrant = grids->GLLoD->bigQuad;
    
    for (unsigned int i = 0; i < game->entities_created_count; i++) {
        int column = (int)floor((entities[i].positionX) / game->low_LOD_quadrant_size);
        int X = column * game->low_LOD_quadrant_size;
        int row = (int)floor((entities[i].positionY) / game->low_LOD_quadrant_size);
        int Y = row * game->low_LOD_quadrant_size;
        
        int indexer = row * amountX + column;
        
        if (indexer < 0 || indexer >= amountX * amountY) {
            printf("indexer = %d, column = %d, X = %d, row = %d, Y = %d\n", indexer, column, X, row, Y);
        }

        if (entities[i].currentQuadrants[0] != quadrant[indexer].id) {
            entities[i].currentQuadrants[0] = 0; // aqui da de ver, quando quadrant 0 mudar, zerar os outros quads
        }

        // else's clears trackers' records
        if (indexer + 1 < amountX * amountY && indexer + 1 != (row + 1) * amountX) {
            entities[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD2);
            // if exiting previous quadrant
            if ((entities[i].currentQuadrants[1] != 0 &&
                entities[i].currentQuadrants[1] != quadrant[indexer + 1].id) ||
                (entities[i].currentQuadrants[1] != 0 &&
                entities[i].positionX + entities[i].dimensionX < X + game->low_LOD_quadrant_size)) {
                quadrant[entities[i].currentQuadrants[1] - 1].entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[1] = 0;
                //entities[i].alreadyInQuadrant &= ~QUAD2;
            }
        } else {
            if(entities[i].currentQuadrants[1] != 0) {
                quadrant[entities[i].currentQuadrants[1] - 1].entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[1] = 0;
                entities[i].quadrantOutOfBounds |= OOBQUAD2;
                //entities[i].alreadyInQuadrant &= ~QUAD2;
            }
        }

        if (indexer + amountX < amountX * amountY) {
            entities[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD3);
            if ((entities[i].currentQuadrants[2] != 0 &&
                entities[i].currentQuadrants[2] != quadrant[indexer + amountX].id) ||
                (entities[i].currentQuadrants[2] != 0 &&
                entities[i].positionY + entities[i].dimensionY < Y + game->low_LOD_quadrant_size)) {
                quadrant[entities[i].currentQuadrants[2] - 1].entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[2] = 0;
                //entities[i].alreadyInQuadrant &= ~QUAD3;
            }
        } else {
            if(entities[i].currentQuadrants[2] != 0) {
                quadrant[entities[i].currentQuadrants[2] - 1].entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[2] = 0;
                entities[i].quadrantOutOfBounds |= OOBQUAD3;
                //entities[i].alreadyInQuadrant &= ~QUAD3;
            }
        }

        if (indexer + amountX + 1 < amountX * amountY && indexer + amountX + 1 != (row + 2) * amountX) {
            entities[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD4);
            if ((entities[i].currentQuadrants[3] != 0 && entities[i].currentQuadrants[3] != quadrant[indexer + amountX + 1].id) ||
                (entities[i].currentQuadrants[1] == 0 || entities[i].currentQuadrants[2] == 0)) {
                quadrant[entities[i].currentQuadrants[3] - 1].entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[3] = 0;
                //entities[i].alreadyInQuadrant &= ~QUAD4;
            }
        } else {
            if(entities[i].currentQuadrants[3] != 0) {
                quadrant[entities[i].currentQuadrants[3] - 1].entitiesInsideQuadrantById[entities[i].id - 1] = 0;
                entities[i].currentQuadrants[3] = 0;
                entities[i].quadrantOutOfBounds |= OOBQUAD4;
                //entities[i].alreadyInQuadrant &= ~QUAD4;
            }
        }

        // if first time entering quadrant
        if (entities[i].currentQuadrants[0] == 0) {
            quadrant[indexer].entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[0] = quadrant[indexer].id;
            //entities[i].alreadyInQuadrant |= QUAD1;
            //quadrant[indexer].entitiesInQuadrantCount++;
        }

        if (entities[i].currentQuadrants[1] == 0 &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD2) &&
            entities[i].positionX + entities[i].dimensionX > X + game->low_LOD_quadrant_size) {
            quadrant[indexer + 1].entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[1] = quadrant[indexer + 1].id;
            //entities[i].alreadyInQuadrant |= QUAD2;
        }

        if (entities[i].currentQuadrants[2] == 0 &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD3) &&
            entities[i].positionY + entities[i].dimensionY > Y + game->low_LOD_quadrant_size) {
            quadrant[indexer + amountX].entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[2] = quadrant[indexer + amountX].id;
            //entities[i].alreadyInQuadrant |= QUAD3;
        }

        if (entities[i].currentQuadrants[3] == 0 &&
            !(entities[i].quadrantOutOfBounds & OOBQUAD4) &&
            entities[i].currentQuadrants[1] != 0 &&
            entities[i].currentQuadrants[2] != 0) {
            quadrant[indexer + amountX + 1].entitiesInsideQuadrantById[entities[i].id - 1] = entities[i].id;
            entities[i].currentQuadrants[3] = quadrant[indexer + amountX + 1].id;
            //entities[i].alreadyInQuadrant |= QUAD4;
        }
    }
}

void renderQuadrantsSetup(Armies *armies, GameState *game) {

    Entity *entities = armies->army->battalions->entities;

    for (unsigned int i = 0; i < game->entities_created_count; i++) {

        int column = (int)floor(entities[i].positionX / game->low_LOD_quadrant_size);
        int X = column * game->low_LOD_quadrant_size;
        int row = (int)floor(entities[i].positionY / game->low_LOD_quadrant_size);
        int Y = row * game->low_LOD_quadrant_size;

        entities[i].previousCol = column;
        entities[i].previousRow = row;
        entities[i].previousX = X;
        entities[i].previousY = Y;

        entities[i].quadrantOccupiedX += (float)X - game->offSetX;
        entities[i].quadrantOccupiedY += (float)Y - game->offSetY;

    }
}

// change quadrant rendering from entity rendering it to wuadrant itself be the renderer
void renderQuadrants(Entity *entities, GameState *game, Player *player, SDL_Renderer *renderer) {
// trocaar dps pra pesquisar por quads ocupados, noa calcular por entity
    for (unsigned int i = 0; i < game->entities_created_count; i++) {
        
        int column = (int)floor(entities[i].positionX / game->low_LOD_quadrant_size);
        int X = column * game->low_LOD_quadrant_size;
        int row = (int)floor(entities[i].positionY / game->low_LOD_quadrant_size);
        int Y = row * game->low_LOD_quadrant_size;

        if (entities[i].previousCol != column) {
            entities[i].quadrantOccupiedX += (float)(X - (signed int)entities[i].previousX);
        }
        if (entities[i].previousRow != row) {
            entities[i].quadrantOccupiedY += (float)(Y - (signed int)entities[i].previousY);
        }

        entities[i].quadrantOccupiedX -= player->vxdt + game->LX + game->KX;
        entities[i].quadrantOccupiedY -= player->vydt + game->LY + game->KY;

        entities[i].previousCol = column;
        entities[i].previousRow = row;
        entities[i].previousX = X;
        entities[i].previousY = Y;

        SDL_Rect quadrants1 = { (signed int)entities[i].quadrantOccupiedX, (signed int)entities[i].quadrantOccupiedY, 100, 100 };
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
        SDL_RenderFillRect(renderer, &quadrants1);


        if (entities[i].currentQuadrants[1] != 0) {
            SDL_Rect quadrants2 = { (signed int)entities[i].quadrantOccupiedX + 100, (signed int)entities[i].quadrantOccupiedY, 100, 100 };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
            SDL_RenderFillRect(renderer, &quadrants2);
        }
        
        if (entities[i].currentQuadrants[2] != 0) {
            SDL_Rect quadrants3 = { (signed int)entities[i].quadrantOccupiedX, (signed int)entities[i].quadrantOccupiedY + 100, 100, 100 };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
            SDL_RenderFillRect(renderer, &quadrants3);
        }
        
        if (entities[i].currentQuadrants[3] != 0) {
            SDL_Rect quadrants4 = { (signed int)entities[i].quadrantOccupiedX + 100, (signed int)entities[i].quadrantOccupiedY + 100, 100, 100 };
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
            SDL_RenderFillRect(renderer, &quadrants4);
        }

    }    
}