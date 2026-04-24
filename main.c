#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include "objects.h"
#include "player.h"
#include "objectMaker.h"
#include "playerMapEdgeCollisionFunc.h"
#include "gameState.h"
#include "maps.h"
#include "mapMaker.h"
#include "windowSettings.h"
#include "playerObjectCollision.h"
#include "entities.h"
#include "entityMaker.h"
#include "calculateEntityMovement.h"
#include "teamsPositionAssembler.h"
#include "quadrant.h"
#include "calculateEntityQuadrant.h"
#include "entityEngageEnemy.h"

int main(void) {
    srand(time(NULL));
    /*
    struct timespec start, end;
    double elapsedNs;
    double totalNs = 0.0;
    unsigned long long countElapsed = 0;
    */
    make_maps();
    Map *map = get_map(map_id[0]);
    
    int player_id = create_player(50 ,100, "lipe", 100.0f, 100.0f, 30.0f, 30.0f, true, 4, 150.0f, 2.4f);
    Player *get_ply = get_player(player_id);

    GameState gameState = {0};
    GameState *game = &gameState;

    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        return 1;
    }
    SDL_Window *win = SDL_CreateWindow(
        "C Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WINDOW_SIZE_X, WINDOW_SIZE_Y,
        SDL_WINDOW_SHOWN
    );
    if (!win) {
        SDL_Log("SDL_CreateWindow error: %s", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_Renderer *ren = SDL_CreateRenderer(
        win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC
    );

    SDL_SetRenderDrawBlendMode(ren, SDL_BLENDMODE_BLEND);

    if (!ren) {
        SDL_Log("SDL_CreateRenderer error: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    bool running = true;


    

    
    // offSet moves map, entities, objects, anything other than the player,
    // an amount base on the difference between player spawn and the center window.
    game->offSetX = get_ply->playerPositionX - WINDOW_SIZE_X / 2 + get_ply->playerDimensionX / 2;
    game->offSetY = get_ply->playerPositionY - WINDOW_SIZE_Y / 2 + get_ply->playerDimensionY / 2;





    make_objects();

    unsigned int i, x, y;

    for (i = 0, x = 0; i < MAX_OBJECTS; i++) {
        if (object_id[i] < 0) break;
        Object *obj = get_object(object_id[i]);
        if (obj == NULL) break;
        x++;
    }

    init_entity_id();
    init_entities();
    //make_entities();
    auto_make_entities(8);
    //team_blue_entities(50);
    //team_red_entities(50);
    //set_team_position(entities);

    for (i = 0, y = 0; i < MAX_ENTITIES; i++) {
        if (entity_id[i] < 0) break;
        Entity *get_ent = get_entity(entity_id[i]);
        if (get_ent == NULL) break;
        get_ent->nextMoveDelay = 10000 - (-5000 + rand() % 5000);
        y++;
    }

    Entity *trimmedEntities = calloc(y, sizeof *trimmedEntities);
    for (int i = 0; i < y; i++) {
        trimmedEntities[i] = entities[i];
    }
    
    //free(entities);
    entities = trimmedEntities;
    trimmedEntities = NULL;
    //free(entity_id);
    entity_id = NULL;

    calculateQuadrantSize(map, game);
    calculateAmountOfQuadrants(map, game);
    init_quadrants(amountX, amountY);
    initialCheckEntityQuadrant(game, entities, quadrant, y);
    renderQuadrantsSetup(ren, game, entities, quadrant, y);
    
    float hitBoxPlayer[4][2] = {
        {get_ply->playerPositionX, get_ply->playerPositionY},
        {get_ply->playerPositionX + get_ply->playerDimensionX, get_ply->playerPositionY},
        {get_ply->playerPositionX, get_ply->playerPositionY + get_ply->playerDimensionY},
        {get_ply->playerPositionX + get_ply->playerDimensionX, get_ply->playerPositionY + get_ply->playerDimensionY}
    };
    
    float hitBoxMaxObjects[MAX_OBJECTS][4][2];

    for (int i = 0; i < x; i++) {
        Object *obj = get_object(object_id[i]);
        hitBoxMaxObjects[i][0][0] = obj->pointX;
        hitBoxMaxObjects[i][0][1] = obj->pointY;
        hitBoxMaxObjects[i][1][0] = obj->pointX + obj->dimensionX;
        hitBoxMaxObjects[i][1][1] = obj->pointY;
        hitBoxMaxObjects[i][2][0] = obj->pointX;
        hitBoxMaxObjects[i][2][1] = obj->pointY + obj->dimensionY;
        hitBoxMaxObjects[i][3][0] = obj->pointX + obj->dimensionX;
        hitBoxMaxObjects[i][3][1] = obj->pointY + obj->dimensionY;
    }

    float (*hitBoxObject)[4][2] = calloc(x, sizeof *hitBoxMaxObjects);

    for (int i = 0; i < x; i++) {
        Object *obj = get_object(object_id[i]);
        hitBoxObject[i][0][0] = obj->pointX;
        hitBoxObject[i][0][1] = obj->pointY;
        hitBoxObject[i][1][0] = obj->pointX + obj->dimensionX;
        hitBoxObject[i][1][1] = obj->pointY;
        hitBoxObject[i][2][0] = obj->pointX;
        hitBoxObject[i][2][1] = obj->pointY + obj->dimensionY;
        hitBoxObject[i][3][0] = obj->pointX + obj->dimensionX;
        hitBoxObject[i][3][1] = obj->pointY + obj->dimensionY;
    }
    
    Uint64 FrameStart = SDL_GetPerformanceCounter();
    Uint64 FrameEnd;
    Uint64 FrameTicks;
    Uint32 now1 = SDL_GetTicks();
    Uint32 last;
    while (running) {
        //clock_gettime(CLOCK_MONOTONIC, &start);
        FrameEnd = SDL_GetPerformanceCounter();
        FrameTicks = SDL_GetPerformanceFrequency();
        game->delta = (float)(FrameEnd - FrameStart) / (float)FrameTicks;
        FrameStart = FrameEnd;
        //printf("delta=%f\n", game->delta);
        //game->delta = 0.016666667f;
        // Calculate game state
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP    : game->dirUp    = 0;
                        break;
                    case SDLK_DOWN  : game->dirDown  = 0;
                        break;
                    case SDLK_LEFT  : game->dirLeft  = 0;
                        break;
                    case SDLK_RIGHT : game->dirRight = 0;
                        break;
                }
            }
            
            if (e.type == SDL_KEYDOWN){
                switch (e.key.keysym.sym) {
                    case SDLK_UP    : game->dirUp    = -get_ply->speed;
                        break;
                    case SDLK_DOWN  : game->dirDown  =  get_ply->speed;
                        break;
                    case SDLK_LEFT  : game->dirLeft  = -get_ply->speed;
                        break;
                    case SDLK_RIGHT : game->dirRight =  get_ply->speed;
                        break;
                }
            }
        }

        // this part checks for whether player can still sprint
        game->vx = game->dirLeft  + game->dirRight;
        game->vy = game->dirUp    + game->dirDown;
        game->va = game->vx + game->vy;
        game->vb = game->vx * game->vy;
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        if (keys[SDL_SCANCODE_LSHIFT] && get_ply->current_st > 0) {
            if (game->vx != 0 || game->vy != 0) {
                get_ply->current_st--;
                game->vx *= get_ply->runSpeed;
                game->vy *= get_ply->runSpeed;
            }
            else if (get_ply->current_st < get_ply->max_st) {
                get_ply->current_st++;
            }
        }
        else if (get_ply->current_st < get_ply->max_st) {
            if (!keys[SDL_SCANCODE_LSHIFT] || game->va == 0 && game->vb == 0) {
                get_ply->current_st++;
            }
        }

        // diagonal movement
        if (game->vy != 0 && game->vx != 0) {
            game->vx *= INVSQRT2;
            game->vy *= INVSQRT2;
        }

        game->vxdt = game->vx * game->delta;
        game->vydt = game->vy * game->delta;
        

        // verifies if player hits corner so to stop object from moving

        PlayerMapEdgeCollision(get_ply, map, game);


        isPlayerCollidingObject(hitBoxPlayer, hitBoxObject, game, x);


        game->offSetX += game->vxdt + game->LX + game->KX;
        game->offSetY += game->vydt + game->LY + game->KY;


        get_ply->playerPositionX += game->vxdt + game->LX + game->KX;
        get_ply->playerPositionY += game->vydt + game->LY + game->KY;

        hitBoxPlayer[0][0] = get_ply->playerPositionX;
        hitBoxPlayer[0][1] = get_ply->playerPositionY;

        hitBoxPlayer[1][0] = get_ply->playerPositionX + get_ply->playerDimensionX;
        hitBoxPlayer[1][1] = get_ply->playerPositionY;

        hitBoxPlayer[2][0] = get_ply->playerPositionX;
        hitBoxPlayer[2][1] = get_ply->playerPositionY + get_ply->playerDimensionY;

        hitBoxPlayer[3][0] = get_ply->playerPositionX + get_ply->playerDimensionX;
        hitBoxPlayer[3][1] = get_ply->playerPositionY + get_ply->playerDimensionY;


        /*Uint32 now2 = SDL_GetTicks();
        for (i = 0; i < y; i++) {
            calculateEntityRandomMov(&entities[i], map, game, now1, now2);
        }*/

        checkEntityQuadrant(game, entities, quadrant, y);
        entityFindEnemy(entities, quadrant, y, q);
        entityEngageEnemy(entities, game, map, y);



        //printf("offSetX= %f, offSetY=%f, LX= %f, LY= %f, mll= %f, ppX= %f\n", game->offSetX, game->offSetY, game->LX, game->LY, map->mapLeftLimit, get_ply->playerPositionX);
        // Render

        SDL_SetRenderDrawColor(ren, 120, 80, 80, 255);

        SDL_RenderClear(ren);

        SDL_Rect Map = {map->mapLeftLimit - game->offSetX, map->mapTopLimit - game->offSetY, map->mapSizeX, map->mapSizeY};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &Map);

        SDL_Rect PlayerRender = { get_ply->playerPositionOnScreenX, get_ply->playerPositionOnScreenY, get_ply->playerDimensionX, get_ply->playerDimensionY };
        SDL_SetRenderDrawColor(ren, 255, 104, 230, 255);
        SDL_RenderFillRect(ren, &PlayerRender);

        for (i = 0; i < x; i++) {
            Object *obj = get_object(object_id[i]);
            if (obj->id == 0) break;
            SDL_Rect Object_Render = {obj->pointX - game->offSetX, obj->pointY - game->offSetY, obj->dimensionX, obj->dimensionY};
            SDL_SetRenderDrawColor(ren, obj->R_Color, obj->G_Color, obj->B_Color, obj->Alpha);
            SDL_RenderFillRect(ren, &Object_Render);

        }

        for (i = 0; i < y; i++) {
            SDL_Rect EntityMovement = { entities[i].positionX - game->offSetX, entities[i].positionY - game->offSetY, entities[i].dimensionX, entities[i].dimensionY};
            SDL_SetRenderDrawColor(ren, entities[i].R_Color, entities[i].G_Color, entities[i].B_Color, entities[i].Alpha);
            SDL_RenderFillRect(ren, &EntityMovement);
        }

        SDL_Rect hp_bar = { 28, 28, get_ply->max_hp + 4, 19 };
        SDL_SetRenderDrawColor(ren, 204, 204, 255, 255);
        SDL_RenderFillRect(ren, &hp_bar);
        
        SDL_Rect hp_empty_bar = { 30, 30, get_ply->max_hp, 15 };
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &hp_empty_bar);

        SDL_Rect hp = { 30, 30, get_ply->current_hp, 15 };
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &hp);

        SDL_Rect st_bar = { 28, 50, get_ply->max_st + 4, 19 };
        SDL_SetRenderDrawColor(ren, 204, 204, 255, 255);
        SDL_RenderFillRect(ren, &st_bar);

        SDL_Rect st_empty_bar = { 30, 52, get_ply->max_st, 15 };
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &st_empty_bar);

        SDL_Rect st = { 30, 52, get_ply->current_st, 15 };
        SDL_SetRenderDrawColor(ren, 102, 255, 51, 255);
        SDL_RenderFillRect(ren, &st);

        renderQuadrants(ren, game, entities, quadrant, y);

        SDL_RenderPresent(ren);

        game->KX = 0;
        game->KY = 0;
        game->LX = 0;
        game->LY = 0;
        /*
        countElapsed++;
        clock_gettime(CLOCK_MONOTONIC, &end);
        elapsedNs =
            (double)(end.tv_sec - start.tv_sec) * 1000000000.0 +
            (double)(end.tv_nsec - start.tv_nsec);
        totalNs += elapsedNs;
        printf("Average: %f ns\n", totalNs / countElapsed);
        */
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
