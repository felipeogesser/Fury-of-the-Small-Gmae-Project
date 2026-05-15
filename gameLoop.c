#include "gameLoop.h"
#include "armies.h"
#include "battalion.h"
#include "calculateEntityQuadrant.h"
#include "engine.h"
#include "entities.h"
#include "gameState.h"
#include "grids.h"
#include "main_menu.h"
#include "maps.h"
#include "peripherals.h"
#include "player.h"
#include "playerMapEdgeCollisionFunc.h"
#include "renderer.h"
#include "update_data.h"
#include "window.h"
#include <SDL2/SDL.h>

void game_loop(void) {

    Armies *armies = engine.armies;
    //Battalion *battalions = engine.armies->army->battalions;
    Entity *entities = engine.armies->army->battalions->entities;
    GameState *game = engine.game;
    Grids *grids = engine.grids;
    Map *map = engine.map;
    Player *player = engine.player;
    SDL_Renderer *renderer = engine.renderer;

    game->scene_state = MAIN_MENU;

    Uint64 FrameStart = SDL_GetPerformanceCounter();
    engine.FrameStart = FrameStart;
    Uint64 FrameEnd;
    Uint64 FrameTicks;

    _Bool *window_running = &engine.window_running;

    while (*window_running) {

        FrameEnd = SDL_GetPerformanceCounter();
        engine.FrameEnd = FrameEnd;
        FrameTicks = SDL_GetPerformanceFrequency();
        engine.FrameTicks = FrameTicks;
        game->delta = (float)(FrameEnd - FrameStart) / (float)FrameTicks;
        FrameStart = FrameEnd;

        switch(game->scene_state) {

            case MAIN_MENU:
            
                //process_peripherals(game, player, window_running);

                render_main_menu_screen(main_menu, renderer);

                continue;

            case BATTLEFIELD:
                
                process_peripherals(game, player, window_running);

                calculate_player_movement(game);
                
                player_map_edge_collision(game, map, player);

                check_entity_quadrant(armies, game, grids);

                update_game_data(entities, game, player);

                render(armies, game, map, player, renderer);

                continue;

        }

    }
    
    destroy_window();


}
