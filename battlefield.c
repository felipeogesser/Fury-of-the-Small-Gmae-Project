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

_Bool show_quads = 0;

void battlefield_init(void) {

    camera_init();
    animation_init();

    init_general_battalion_taxonomy();

    make_maps();
    engine.map = get_map(map_id[0]);
    unsigned int player_id = create_player(50 ,1000, "lipe", 100.0f, 100.0f, 30.0f, 30.0f, true, 4, 150.0f, 2.4f);
    engine.player = get_player(player_id);
    init_armies_memory_arena();
    load_armies_into_arena();
    calculateAmountOfQuadrants();
    init_grids();
    initialCheckEntityQuadrant(engine.armies, engine.game, engine.grids);
    renderQuadrantsSetup(engine.armies, engine.game);

}

void battlefield_input(SDL_Event *e) {

    if (e->key.keysym.sym == SDLK_1) show_quads = !show_quads;
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

    update_game_state();

}

void battlefield_render(void) {

    Armies *armies = engine.armies;
    Map *map = engine.map;
    Player *player = engine.player;
    SDL_Renderer *renderer = engine.renderer;
            
    //SDL_SetRenderDrawColor(renderer, 120, 80, 80, 255);

    //SDL_RenderClear(renderer);

    SDL_Rect map_rect = {
        (signed int)map->mapLeftLimit,
        (signed int)map->mapTopLimit,
        (signed int)map->mapSizeX,
        (signed int)map->mapSizeY};
    camera_world_to_screen(&map_rect);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderFillRect(renderer, &map_rect);

    SDL_Rect player_render = {
        (signed int)player->playerPositionX,
        (signed int)player->playerPositionY,
        (signed int)player->playerDimensionX,
        (signed int)player->playerDimensionY};
    camera_world_to_screen(&player_render);
    SDL_SetRenderDrawColor(renderer, 255, 104, 230, 255);
    SDL_RenderFillRect(renderer, &player_render);

    unsigned int number_of_armies = armies->number_of_armies;
    unsigned int battalion_count = armies->army->battalion_count;

    Battalion *battalions = armies->army->battalions;
    unsigned int entities_count = battalions->entities_count;

    for (unsigned int i = 0; i < number_of_armies * battalion_count; i++) {
        
        Entity *entities = battalions[i].entities;
        General *general = armies->army->general;
        
            SDL_Rect general_render = {
                (signed int)general[i].positionX,
                (signed int)general[i].positionY,
                (signed int)general[i].dimensionX,
                (signed int)general[i].dimensionY};
            camera_world_to_screen(&general_render);
            SDL_SetRenderDrawColor(renderer, general[i].R_color, general[i].G_color, general[i].B_color, general[i].Alpha);
            SDL_RenderFillRect(renderer, &general_render);

        for (unsigned int j = 0; j < entities_count; j++) {

            SDL_Rect entities_render = {
                (signed int)entities[j].positionX,
                (signed int)entities[j].positionY,
                (signed int)entities[j].dimensionX,
                (signed int)entities[j].dimensionY};
            camera_world_to_screen(&entities_render);
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

    if (show_quads) {
        Entity *entities = battalions->entities;
        GameState *game = engine.game;
        renderQuadrants(entities, game, renderer);
    }

}

void battlefield_destroy(void) {

    animation_destroy();

    memory_arena_reset();

}