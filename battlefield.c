#include "battlefield.h"
#include "animation.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "camera.h"
#include "camera_internal.h"
#include "engine_internal.h"
#include "entity_collision.h"
#include "entities.h"
#include "entities_internal.h"
#include "game_state.h"
#include "game_state_internal.h"
#include "general.h"
#include "general_internal.h"
#include "grids.h"
#include "load_armies.h"
#include "memory_arena.h"
#include "mapMaker.h"
#include "maps.h"
#include "maps_internal.h"
#include "player.h"
#include "player_internal.h"
#include "player_collision.h"
#include "quadrant.h"
#include "quadrant_internal.h"
#include "window_settings.h"
#include <SDL2/SDL.h>
#include <stdbool.h>

void battlefield_init(void) {

    camera_init();
    animation_init();

    init_general_battalion_taxonomy();

    make_maps();
    engine.map = get_map(map_id[0]);
    unsigned int player_id = create_player(50 ,1000, "lipe", 100.0f, 100.0f, 30.0f, 30.0f, true, 4, 150.0f, 2.4f);
    engine.player = get_player(player_id);
    //engine.game->offSetX = engine.player->playerPositionX - WINDOW_SIZE_X / 2 + engine.player->playerDimensionX / 2;
    //engine.game->offSetY = engine.player->playerPositionY - WINDOW_SIZE_Y / 2 + engine.player->playerDimensionY / 2;

    init_armies_memory_arena();
    load_armies_into_arena();
    calculateAmountOfQuadrants();
    init_grids();
    initialCheckEntityQuadrant(engine.armies, engine.game, engine.grids);
    renderQuadrantsSetup(engine.armies, engine.game);

}

void battlefield_input(SDL_Event *e) {

    Player *player = engine.player;
        
    if (e->type == SDL_KEYUP) {
        switch (e->key.keysym.sym) {
            case SDLK_UP    : player->dir_up    = 0;
                break;
            case SDLK_DOWN  : player->dir_down  = 0;
                break;
            case SDLK_LEFT  : player->dir_left  = 0;
                break;
            case SDLK_RIGHT : player->dir_right = 0;
                break;
        }
    }
    
    if (e->type == SDL_KEYDOWN){
        switch (e->key.keysym.sym) {
            case SDLK_UP    : player->dir_up    = -player->speed;
                break;
            case SDLK_DOWN  : player->dir_down  =  player->speed;
                break;
            case SDLK_LEFT  : player->dir_left  = -player->speed;
                break;
            case SDLK_RIGHT : player->dir_right =  player->speed;
                break;
        }
    }

    camera_input(e);
    animation_input(e);

}

void battlefield_update(void) {
    
    calculate_player_movement(engine.game);
    
    player_map_edge_collision(engine.game, engine.map, engine.player);

    check_entity_quadrant(engine.armies, engine.game, engine.grids);

    update_player(engine.game);

    update_units(engine.armies, engine.game);

    update_generals(engine.armies, engine.game);

    camera_update();

    animation_update();

    update_game_state(engine.player);

}

void battlefield_render(void) {

    Armies *armies = engine.armies;
    //GameState *game = engine.game;
    Map *map = engine.map;
    Player *player = engine.player;
    SDL_Renderer *renderer = engine.renderer;
            
    //SDL_SetRenderDrawColor(renderer, 120, 80, 80, 255);

    //SDL_RenderClear(renderer);

    float screen_pos_x1, screen_pos_y1;
    camera_world_to_screen(
        map->mapLeftLimit, map->mapTopLimit,
        &screen_pos_x1, &screen_pos_y1);

    SDL_Rect map_rect = {(signed int)screen_pos_x1/* - (signed int)game->offSetX*/, (signed int)screen_pos_y1/* - (signed int)game->offSetY*/, (signed int)(map->mapSizeX * engine.camera->zoom), (signed int)(map->mapSizeY * engine.camera->zoom)};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &map_rect);

    float screen_pos_x, screen_pos_y;
    camera_world_to_screen(
        player->playerPositionX, player->playerPositionY,
        &screen_pos_x, &screen_pos_y);

    SDL_Rect PlayerRender = { (signed int)screen_pos_x, (signed int)screen_pos_y, (signed int)(player->playerDimensionX * engine.camera->zoom), (signed int)(player->playerDimensionY * engine.camera->zoom)};
    SDL_SetRenderDrawColor(renderer, 255, 104, 230, 255);
    SDL_RenderFillRect(renderer, &PlayerRender);

    unsigned int number_of_armies = armies->number_of_armies;
    unsigned int battalion_count = armies->army->battalion_count;

    Battalion *battalions = armies->army->battalions;
    unsigned int entities_count = battalions->entities_count;

    for (unsigned int i = 0; i < number_of_armies * battalion_count; i++) {
        
        Entity *entities = battalions[i].entities;
        General *general = armies->army->general;
        
            float screen_pos_x2, screen_pos_y2;
            camera_world_to_screen(
                general[i].positionX, general[i].positionY,
                &screen_pos_x2, &screen_pos_y2);

            SDL_Rect general_render = { (signed int)(screen_pos_x2 /*- game->offSetX*/), (signed int)(screen_pos_y2 /*- game->offSetY*/), (signed int)(general[i].dimensionX * engine.camera->zoom), (signed int)(general[i].dimensionY * engine.camera->zoom)};
            SDL_SetRenderDrawColor(renderer, general[i].R_color, general[i].G_color, general[i].B_color, general[i].Alpha);
            SDL_RenderFillRect(renderer, &general_render);

        for (unsigned int j = 0; j < entities_count; j++) {
            
            float screen_pos_x3, screen_pos_y3;
            camera_world_to_screen(
                entities[j].positionX, entities[j].positionY,
                &screen_pos_x3, &screen_pos_y3);

            SDL_Rect entities_render = { (signed int)(screen_pos_x3 + 10 * engine.camera->zoom/* - game->offSetX*/), (signed int)(screen_pos_y3 + 10 * engine.camera->zoom/* - game->offSetY*/), (signed int)(entities[j].dimensionX * engine.camera->zoom), (signed int)(entities[j].dimensionY * engine.camera->zoom)};
            SDL_SetRenderDrawColor(renderer, battalions[i].R_Color, battalions[i].G_Color, battalions[i].B_Color, battalions[i].Alpha);
            SDL_RenderFillRect(renderer, &entities_render);

        }
    //mudar dps. fazer todos o generais antes, dps todas as units para n perder pre fetch
    }

    SDL_Rect hp_bar = { 28, 28, player->max_hp + 4, 19 };
    SDL_SetRenderDrawColor(renderer, 204, 204, 255, 255);
    SDL_RenderFillRect(renderer, &hp_bar);
    
    SDL_Rect hp_empty_bar = { 30, 30, player->max_hp, 15 };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hp_empty_bar);

    SDL_Rect hp = { 30, 30, player->current_hp, 15 };
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderFillRect(renderer, &hp);

    SDL_Rect st_bar = { 28, 50, player->max_st + 4, 19 };
    SDL_SetRenderDrawColor(renderer, 204, 204, 255, 255);
    SDL_RenderFillRect(renderer, &st_bar);

    SDL_Rect st_empty_bar = { 30, 52, player->max_st, 15 };
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &st_empty_bar);

    SDL_Rect st = { 30, 52, player->current_st, 15 };
    SDL_SetRenderDrawColor(renderer, 102, 255, 51, 255);
    SDL_RenderFillRect(renderer, &st);

    animation_render();

    //Entity *entities = battalions->entities;
    //GameState *game = engine.game;
    //renderQuadrants(entities, game, renderer);

    //SDL_RenderPresent(renderer);

}

void battlefield_destroy(void) {

    animation_destroy();

    memory_arena_reset();

}