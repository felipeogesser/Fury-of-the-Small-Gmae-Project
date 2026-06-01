#include "battlefield.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "engine_internal.h"
#include "entity_collision.h"
#include "entities.h"
#include "entities_internal.h"
#include "game_state.h"
#include "game_state_internal.h"
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

    make_maps();
    engine.map = get_map(map_id[0]);
    unsigned int player_id = create_player(50 ,1000, "lipe", 100.0f, 100.0f, 30.0f, 30.0f, true, 4, 150.0f, 2.4f);
    engine.player = get_player(player_id);
    engine.game->offSetX = engine.player->playerPositionX - WINDOW_SIZE_X / 2 + engine.player->playerDimensionX / 2;
    engine.game->offSetY = engine.player->playerPositionY - WINDOW_SIZE_Y / 2 + engine.player->playerDimensionY / 2;

    init_armies_memory_arena();
    load_armies_into_arena();
    calculateAmountOfQuadrants();
    init_grids();
    fill_quadrant_data();
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
}

void battlefield_update(void) {

    calculate_player_movement(engine.game);
    
    player_map_edge_collision(engine.game, engine.map, engine.player);

    check_entity_quadrant(engine.armies, engine.game, engine.grids);

    update_player(engine.game);

    update_units(engine.armies, engine.game);

    update_game_state(engine.player);

}

void battlefield_render(void) {

    Armies *armies = engine.armies;
    GameState *game = engine.game;
    Map *map = engine.map;
    Player *player = engine.player;
    SDL_Renderer *renderer = engine.renderer;
            
    SDL_SetRenderDrawColor(renderer, 120, 80, 80, 255);

    SDL_RenderClear(renderer);

    SDL_Rect map_rect = {map->mapLeftLimit - (signed int)game->offSetX, map->mapTopLimit - (signed int)game->offSetY, map->mapSizeX, map->mapSizeY};
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &map_rect);

    SDL_Rect PlayerRender = { (signed int)player->playerPositionOnScreenX, (signed int)player->playerPositionOnScreenY, (signed int)player->playerDimensionX, (signed int)player->playerDimensionY };
    SDL_SetRenderDrawColor(renderer, 255, 104, 230, 255);
    SDL_RenderFillRect(renderer, &PlayerRender);

    unsigned int number_of_armies = armies->number_of_armies;
    unsigned int battalion_count = armies->army->battalion_count;

    Battalion *battalions = armies->army->battalions;
    unsigned int entities_count = battalions->entities_count;

    for (unsigned int i = 0; i < number_of_armies * battalion_count; i++) {
        
        Entity *entities = battalions[i].entities;
        
        for (unsigned int j = 0; j < entities_count; j++) {
            
            SDL_Rect entities_render = { (signed int)(entities[j].positionX - game->offSetX), (signed int)(entities[j].positionY - game->offSetY), (signed int)entities[j].dimensionX, (signed int)entities[j].dimensionY};
            SDL_SetRenderDrawColor(renderer, battalions[i].R_Color, battalions[i].G_Color, battalions[i].B_Color, battalions[i].Alpha);
            SDL_RenderFillRect(renderer, &entities_render);

        }

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

    //Entity *entities = battalions->entities;

    //renderQuadrants(entities, game, renderer);

    SDL_RenderPresent(renderer);

}

void battlefield_destroy(void) {

    memory_arena_reset();

}