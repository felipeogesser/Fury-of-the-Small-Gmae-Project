#include "player.h"
#include "engine.h"

static Player player[MAX_PLAYERS] = {0};

/*void init_player(void) {
    engine.player = player;
}*/

static int player_count = 0;

int create_player(int max_hp, int max_st, const char *name,
    float playerSpawnX, float playerSpawnY,
    float playerDimensionX, float playerDimensionY,
    bool collision, int quadrant,
    float speed, float runSpeed) {
    if (player_count >= MAX_PLAYERS) return -1; 
    unsigned int playerIndex = player_count;
    player[playerIndex].id = playerIndex + 1;
    player[playerIndex].max_hp = max_hp;
    player[playerIndex].max_st = max_st;
    player[playerIndex].current_hp = max_hp;
    player[playerIndex].current_st = max_st;
    snprintf(player[playerIndex].name, sizeof player[playerIndex].name, "%s", name ? name : "");
    player[playerIndex].playerSpawnX = playerSpawnX;
    player[playerIndex].playerSpawnY = playerSpawnY;
    player[playerIndex].playerPositionX = playerSpawnX;
    player[playerIndex].playerPositionY = playerSpawnY;
    player[playerIndex].playerPositionOnScreenX = WINDOW_SIZE_X / 2 - playerDimensionX / 2;
    player[playerIndex].playerPositionOnScreenY = WINDOW_SIZE_Y / 2 - playerDimensionY / 2;
    player[playerIndex].playerDimensionX = playerDimensionX;
    player[playerIndex].playerDimensionY = playerDimensionY;
    player[playerIndex].collision = collision;
    player[playerIndex].quadrant = quadrant;
    player[playerIndex].speed = speed;
    player[playerIndex].runSpeed = runSpeed;
    player_count++;
    return player[playerIndex].id;
}

Player *get_player(int playerId) {
    if (playerId <= 0 || playerId > player_count) return NULL;
    return &player[playerId - 1];
}

void calculate_player_movement(GameState *game) {
    
    // orthogonal movement
    game->vx = game->dirLeft  + game->dirRight;
    game->vy = game->dirUp    + game->dirDown;
    game->va = game->vx + game->vy;
    game->vb = game->vx * game->vy;
    const Uint8 *keys = SDL_GetKeyboardState(NULL);
    
    // can player sprint?
    if (keys[SDL_SCANCODE_LSHIFT] && player->current_st > 0) {
        if (game->vx != 0 || game->vy != 0) {
            player->current_st--;
            game->vx *= player->runSpeed;
            game->vy *= player->runSpeed;
        }
        else if (player->current_st < player->max_st) {
            player->current_st++;
        }
    }
    else if (player->current_st < player->max_st) {
        if ((!keys[SDL_SCANCODE_LSHIFT]) || (game->va == 0 && game->vb == 0)) {
            player->current_st++;
        }
    }

    // diagonal movement
    if (game->vy != 0 && game->vx != 0) {
        game->vx *= INVSQRT2;
        game->vy *= INVSQRT2;
    }

    game->vxdt = game->vx * game->delta;
    game->vydt = game->vy * game->delta;
    
}