#include "renderer.h"
#include "armies_internal.h"
#include "battalion_internal.h"
#include "unit_internal.h"
#include "game_state_internal.h"
#include "maps_internal.h"
#include "player_internal.h"
//#include "scene_handler.h"


void render(Armies *armies, GameState *game, Map *map, Player *player, SDL_Renderer *renderer) {
    
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
    unsigned int unit_count = battalions->unit_count;

    for (unsigned int i = 0; i < number_of_armies * battalion_count; i++) {
        
        Unit *unit = battalions[i].unit;
        
        for (unsigned int j = 0; j < unit_count; j++) {
            
            SDL_Rect UnitMovement = { (signed int)(unit[j].positionX - game->offSetX), (signed int)(unit[j].positionY - game->offSetY), (signed int)unit[j].dimensionX, (signed int)unit[j].dimensionY};
            SDL_SetRenderDrawColor(renderer, battalions[i].R_Color, battalions[i].G_Color, battalions[i].B_Color, battalions[i].Alpha);
            SDL_RenderFillRect(renderer, &UnitMovement);

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

    //Unit *unit = battalions->unit;

    //renderQuadrants(unit, game, renderer);

    SDL_RenderPresent(renderer);
}