#include "player.h"
#include "player_internal.h"
#include "game_state_internal.h"
//#include "engine_internal.h"
#include "window_settings.h"
#include <stdio.h>
#include <stdbool.h>
#include <SDL2/SDL.h>

static Player player = {0};

/*void init_player(void) {
    engine.player = &player;
}*/

static unsigned int player_count = 0;

unsigned int create_player(int max_hp, int max_st, const char *name,
    float playerSpawnX, float playerSpawnY,
    float playerDimensionX, float playerDimensionY,
    _Bool collision, int quadrant,
    float speed, float runSpeed) {
    if (player_count >= MAX_PLAYERS) return 1; 
    unsigned int playerIndex = player_count;
    player.id = playerIndex + 1;
    player.max_hp = max_hp;
    player.max_st = max_st;
    player.current_hp = max_hp;
    player.current_st = max_st;
    snprintf(player.name, sizeof player.name, "%s", name ? name : "");
    player.playerSpawnX = playerSpawnX;
    player.playerSpawnY = playerSpawnY;
    player.playerPositionX = playerSpawnX;
    player.playerPositionY = playerSpawnY;
    player.playerPositionOnScreenX = WINDOW_SIZE_X / 2 - playerDimensionX / 2;
    player.playerPositionOnScreenY = WINDOW_SIZE_Y / 2 - playerDimensionY / 2;
    player.playerDimensionX = playerDimensionX;
    player.playerDimensionY = playerDimensionY;
    player.collision = collision;
    player.quadrant = quadrant;
    player.speed = speed;
    player.runSpeed = runSpeed;
    player_count++;
    return player.id;
}

Player *get_player(unsigned int playerId) {
    if (playerId <= 0 || playerId > player_count) return NULL;
    return &player;
}

void calculate_player_movement(GameState *game) {
    
    // orthogonal movement
    player.vx = player.dir_left  + player.dir_right;
    player.vy = player.dir_up    + player.dir_down;
    player.va = player.vx + player.vy;
    player.vb = player.vx * player.vy;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    // can player sprint?
    if (keys[SDL_SCANCODE_LSHIFT] && player.current_st > 0) {
        if (player.vx != 0 || player.vy != 0) {
            player.current_st--;
            player.vx *= player.runSpeed;
            player.vy *= player.runSpeed;
        }
        else if (player.current_st < player.max_st) {
            player.current_st++;
        }
    }
    else if (player.current_st < player.max_st) {
        if ((!keys[SDL_SCANCODE_LSHIFT]) || (player.va == 0 && player.vb == 0)) {
            player.current_st++;
        }
    }

    // diagonal movement
    if (player.vy != 0 && player.vx != 0) {
        player.vx *= INVSQRT2;
        player.vy *= INVSQRT2;
    }

    player.vxdt = player.vx * game->delta;
    player.vydt = player.vy * game->delta;
    
}

void update_player(GameState *game) {
    
    player.playerPositionX += player.vxdt + game->LX + game->KX;
    player.playerPositionY += player.vydt + game->LY + game->KY;

}