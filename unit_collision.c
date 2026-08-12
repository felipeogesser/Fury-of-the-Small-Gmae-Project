#include "unit_collision.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "camera.h"
#include "camera_internal.h"
#include "engine_internal.h"
#include "unit_internal.h"
#include "game_state_internal.h"
#include "general_internal.h"
#include "grids_internal.h"
#include "load_armies.h"
#include "quadrant_internal.h"
#include <SDL2/SDL.h>

#define QUAD1 ((unsigned char)(1u << 0))
#define QUAD2 ((unsigned char)(1u << 1))
#define QUAD3 ((unsigned char)(1u << 2))
#define QUAD4 ((unsigned char)(1u << 3))

#define OOBQUAD2 ((unsigned char)(1u << 1))
#define OOBQUAD3 ((unsigned char)(1u << 2))
#define OOBQUAD4 ((unsigned char)(1u << 3))

// futuramente trocar [i] por ->

void initialCheckUnitQuadrant(Armies *armies, GameState *game, Grids *grids) {

    Unit *unit = armies->army->battalions->unit;
    signed int amountX = game->amountX;
    signed int amountY = game->amountY;
    bigQuadrant *quadrant = grids->GLLoD->bigQuad;

    for (unsigned int i = 0; i < game->unit_created_count; i++) {
        
        int column = (int)floor(unit[i].positionX / game->low_LOD_quadrant_size);
        int X = column * (signed int)game->low_LOD_quadrant_size;
        int row = (int)floor(unit[i].positionY / game->low_LOD_quadrant_size);
        int Y = row * game->low_LOD_quadrant_size;
 
        signed int indexer = row * amountX + column;

        if (indexer < 0 || indexer >= amountX * amountY) {
            printf("indexer = %d, column = %d, X = %d, row = %d, Y = %d\n", indexer, column, X, row, Y);
        }
        
        quadrant[indexer].unit_inside_quad[unit[i].id - 1] = unit[i].id;
        unit[i].currentQuadrants[0] = quadrant[indexer].id;
        //unit[i].alreadyInQuadrant |= QUAD1;

        unit[i].currentQuadrants[1] = 0;
        unit[i].currentQuadrants[2] = 0;
        unit[i].currentQuadrants[3] = 0;

        if (indexer + 1 < amountX * amountY && indexer + 1 != (row + 1) * amountX) {
            unit[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD2);
        } else {
            unit[i].quadrantOutOfBounds |= OOBQUAD2;
            //unit[i].alreadyInQuadrant &= ~QUAD2;
        }

        if (indexer + amountX < amountX * amountY) {
            unit[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD3);
        } else {
            unit[i].quadrantOutOfBounds |= OOBQUAD3;
            //unit[i].alreadyInQuadrant &= ~QUAD3;
        }
                                                            //indexer + 1 != (row + 1) * amountX
        if (indexer + amountX + 1 < amountX * amountY && indexer + amountX + 1 != (row + 2) * amountX) {
            unit[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD4);
        } else {
            unit[i].quadrantOutOfBounds |= OOBQUAD4;
            //unit[i].alreadyInQuadrant &= ~QUAD4;
        }

        if (!(unit[i].quadrantOutOfBounds & QUAD2) && unit[i].positionX + unit[i].dimensionX > X + game->low_LOD_quadrant_size) {
            quadrant[indexer + 1].unit_inside_quad[unit[i].id - 1] = unit[i].id;
            unit[i].currentQuadrants[1] = quadrant[indexer + 1].id;
            //unit[i].alreadyInQuadrant |= QUAD2;
        }

        if (!(unit[i].quadrantOutOfBounds & QUAD3) && unit[i].positionY + unit[i].dimensionY > Y + game->low_LOD_quadrant_size) {
            quadrant[indexer + amountX].unit_inside_quad[unit[i].id - 1] = unit[i].id;
            unit[i].currentQuadrants[2] = quadrant[indexer + amountX].id;
            //unit[i].alreadyInQuadrant |= QUAD3;
        }

        if (unit[i].currentQuadrants[1] != 0 &&
            unit[i].currentQuadrants[2] != 0) {
            quadrant[indexer + amountX + 1].unit_inside_quad[unit[i].id - 1] = unit[i].id;
            unit[i].currentQuadrants[3] = quadrant[indexer + amountX + 1].id;
            //unit[i].alreadyInQuadrant |= QUAD4;
        }
    }
}




// unit goes into indexes = to their id -1
void check_unit_quadrant(Armies *armies, GameState *game, Grids *grids) {

    Unit *unit = armies->army->battalions->unit;
    signed int amountX = game->amountX;
    signed int amountY = game->amountY;
    bigQuadrant *quadrant = grids->GLLoD->bigQuad;
    
    for (unsigned int i = 0; i < game->unit_created_count; i++) {
        int column = (int)floor((unit[i].positionX) / game->low_LOD_quadrant_size);
        int X = column * game->low_LOD_quadrant_size;
        int row = (int)floor((unit[i].positionY) / game->low_LOD_quadrant_size);
        int Y = row * game->low_LOD_quadrant_size;
        
        int indexer = row * amountX + column;
        
        /*if (indexer < 0 || indexer >= amountX * amountY) {
            printf("indexer = %d, column = %d, X = %d, row = %d, Y = %d\n", indexer, column, X, row, Y);
        }*/

        if (unit[i].currentQuadrants[0] != quadrant[indexer].id) {
            unit[i].currentQuadrants[0] = 0; // aqui da de ver, quando quadrant 0 mudar, zerar os outros quads
        }

        // else's clears trackers' records
        if (indexer + 1 < amountX * amountY && indexer + 1 != (row + 1) * amountX) {
            unit[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD2);
            // if exiting previous quadrant
            if ((unit[i].currentQuadrants[1] != 0 &&
                unit[i].currentQuadrants[1] != quadrant[indexer + 1].id) ||
                (unit[i].currentQuadrants[1] != 0 &&
                unit[i].positionX + unit[i].dimensionX < X + game->low_LOD_quadrant_size)) {
                quadrant[unit[i].currentQuadrants[1] - 1].unit_inside_quad[unit[i].id - 1] = 0;
                unit[i].currentQuadrants[1] = 0;
                //unit[i].alreadyInQuadrant &= ~QUAD2;
            }
        } else {
            if(unit[i].currentQuadrants[1] != 0) {
                quadrant[unit[i].currentQuadrants[1] - 1].unit_inside_quad[unit[i].id - 1] = 0;
                unit[i].currentQuadrants[1] = 0;
                unit[i].quadrantOutOfBounds |= OOBQUAD2;
                //unit[i].alreadyInQuadrant &= ~QUAD2;
            }
        }

        if (indexer + amountX < amountX * amountY) {
            unit[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD3);
            if ((unit[i].currentQuadrants[2] != 0 &&
                unit[i].currentQuadrants[2] != quadrant[indexer + amountX].id) ||
                (unit[i].currentQuadrants[2] != 0 &&
                unit[i].positionY + unit[i].dimensionY < Y + game->low_LOD_quadrant_size)) {
                quadrant[unit[i].currentQuadrants[2] - 1].unit_inside_quad[unit[i].id - 1] = 0;
                unit[i].currentQuadrants[2] = 0;
                //unit[i].alreadyInQuadrant &= ~QUAD3;
            }
        } else {
            if(unit[i].currentQuadrants[2] != 0) {
                quadrant[unit[i].currentQuadrants[2] - 1].unit_inside_quad[unit[i].id - 1] = 0;
                unit[i].currentQuadrants[2] = 0;
                unit[i].quadrantOutOfBounds |= OOBQUAD3;
                //unit[i].alreadyInQuadrant &= ~QUAD3;
            }
        }

        if (indexer + amountX + 1 < amountX * amountY && indexer + amountX + 1 != (row + 2) * amountX) {
            unit[i].quadrantOutOfBounds &= (unsigned char)(~OOBQUAD4);
            if (unit[i].currentQuadrants[3] != 0) {
                if ((unit[i].currentQuadrants[3] != quadrant[indexer + amountX + 1].id) ||
                    (unit[i].currentQuadrants[1] == 0 || unit[i].currentQuadrants[2] == 0)) {
                    quadrant[unit[i].currentQuadrants[3] - 1].unit_inside_quad[unit[i].id - 1] = 0;
                    unit[i].currentQuadrants[3] = 0;
                    //unit[i].alreadyInQuadrant &= ~QUAD4;
                }
            }
        } else {
            if(unit[i].currentQuadrants[3] != 0) {
                quadrant[unit[i].currentQuadrants[3] - 1].unit_inside_quad[unit[i].id - 1] = 0;
                unit[i].currentQuadrants[3] = 0;
                unit[i].quadrantOutOfBounds |= OOBQUAD4;
                //unit[i].alreadyInQuadrant &= ~QUAD4;
            }
        }

        // if first time entering quadrant
        if (unit[i].currentQuadrants[0] == 0) {
            quadrant[indexer].unit_inside_quad[unit[i].id - 1] = unit[i].id;
            unit[i].currentQuadrants[0] = quadrant[indexer].id;
            //unit[i].alreadyInQuadrant |= QUAD1;
            //quadrant[indexer].unitInQuadrantCount++;
        }

        if (unit[i].currentQuadrants[1] == 0 &&
            !(unit[i].quadrantOutOfBounds & OOBQUAD2) &&
            unit[i].positionX + unit[i].dimensionX > X + game->low_LOD_quadrant_size) {
            quadrant[indexer + 1].unit_inside_quad[unit[i].id - 1] = unit[i].id;
            unit[i].currentQuadrants[1] = quadrant[indexer + 1].id;
            //unit[i].alreadyInQuadrant |= QUAD2;
        }

        if (unit[i].currentQuadrants[2] == 0 &&
            !(unit[i].quadrantOutOfBounds & OOBQUAD3) &&
            unit[i].positionY + unit[i].dimensionY > Y + game->low_LOD_quadrant_size) {
            quadrant[indexer + amountX].unit_inside_quad[unit[i].id - 1] = unit[i].id;
            unit[i].currentQuadrants[2] = quadrant[indexer + amountX].id;
            //unit[i].alreadyInQuadrant |= QUAD3;
        }

        if (unit[i].currentQuadrants[3] == 0 &&
            !(unit[i].quadrantOutOfBounds & OOBQUAD4) &&
            unit[i].currentQuadrants[1] != 0 &&
            unit[i].currentQuadrants[2] != 0) {
            quadrant[indexer + amountX + 1].unit_inside_quad[unit[i].id - 1] = unit[i].id;
            unit[i].currentQuadrants[3] = quadrant[indexer + amountX + 1].id;
            //unit[i].alreadyInQuadrant |= QUAD4;
        }
    }
}

void renderQuadrantsSetup(Armies *armies, GameState *game) {

    Unit *unit = armies->army->battalions->unit;

    for (unsigned int i = 0; i < game->unit_created_count; i++) {

        int column = (int)floor(unit[i].positionX / game->low_LOD_quadrant_size);
        int X = column * game->low_LOD_quadrant_size;
        int row = (int)floor(unit[i].positionY / game->low_LOD_quadrant_size);
        int Y = row * game->low_LOD_quadrant_size;

        unit[i].previousCol = column;
        unit[i].previousRow = row;
        unit[i].previousX = X;
        unit[i].previousY = Y;

        unit[i].quadrantOccupiedX += (float)X;
        unit[i].quadrantOccupiedY += (float)Y;

    }
}

// change quadrant rendering from Unit rendering it to quadrant itself be the renderer
void renderQuadrants(Unit *unit, GameState *game, SDL_Renderer *renderer) {
// trocaar dps pra pesquisar por quads ocupados, noa calcular por Unit
    for (unsigned int i = 0; i < game->unit_created_count; i++) {
        
        int column = (int)floor(unit[i].positionX / game->low_LOD_quadrant_size);
        int X = column * game->low_LOD_quadrant_size;
        int row = (int)floor(unit[i].positionY / game->low_LOD_quadrant_size);
        int Y = row * game->low_LOD_quadrant_size;

        if (unit[i].previousCol != column) {
            unit[i].quadrantOccupiedX += (float)(X - (signed int)unit[i].previousX);
        }
        if (unit[i].previousRow != row) {
            unit[i].quadrantOccupiedY += (float)(Y - (signed int)unit[i].previousY);
        }

        unit[i].previousCol = column;
        unit[i].previousRow = row;
        unit[i].previousX = X;
        unit[i].previousY = Y;

        SDL_Rect quadrants1 = {
            (signed int)unit[i].quadrantOccupiedX,
            (signed int)unit[i].quadrantOccupiedY,
            100,
            100};
        camera_world_to_screen(&quadrants1);
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
        SDL_RenderFillRect(renderer, &quadrants1);


        if (unit[i].currentQuadrants[1] != 0) {
            SDL_Rect quadrants2 = {
                (signed int)unit[i].quadrantOccupiedX + 100,
                (signed int)unit[i].quadrantOccupiedY,
                100,
                100};
            camera_world_to_screen(&quadrants2);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
            SDL_RenderFillRect(renderer, &quadrants2);
        }
        
        if (unit[i].currentQuadrants[2] != 0) {
            SDL_Rect quadrants3 = {
                (signed int)unit[i].quadrantOccupiedX,
                (signed int)unit[i].quadrantOccupiedY + 100,
                100,
                100};
            camera_world_to_screen(&quadrants3);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
            SDL_RenderFillRect(renderer, &quadrants3);
        }
        
        if (unit[i].currentQuadrants[3] != 0) {
            SDL_Rect quadrants4 = {
                (signed int)unit[i].quadrantOccupiedX + 100,
                (signed int)unit[i].quadrantOccupiedY + 100,
                100,
                100};
            camera_world_to_screen(&quadrants4);
            SDL_SetRenderDrawColor(renderer, 0, 255, 0, 100);
            SDL_RenderFillRect(renderer, &quadrants4);
        }

    }    
}