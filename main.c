#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "objects.h"
#include "player.h"

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        return 1;
    }
    float WindowSizeX = 800.0f, WindowSizeY = 600.0f;
    SDL_Window *win = SDL_CreateWindow(
        "C Game",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        WindowSizeX, WindowSizeY,
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
    if (!ren) {
        SDL_Log("SDL_CreateRenderer error: %s", SDL_GetError());
        SDL_DestroyWindow(win);
        SDL_Quit();
        return 1;
    }

    bool running = true;

    int wall_id = create_object("wallV", 600.0f, 40.0f, 30.0f, 400.0f, true, 4);
    int player_id = create_player(50 ,100, "lipe", 100.0f, 100.0f, 30.0f, 30.0f, true, 4);
    
    Player *get_ply = get_player(player_id);

    Object *get_obj = get_object(wall_id);


    int PlayerMaxHealth = get_ply->max_hp;
    int PlayerMaxStamina = get_ply->max_st;
    int PlayerCurrentHealth = get_ply->current_hp;
    int PlayerCurrentStamina = get_ply->current_st;
    float PlayerPositionX = get_ply->spawnX;
    float PlayerPositionY = get_ply->spawnY;
    float PlayerDimensionX = get_ply->dimensionX;
    float PlayerDimensionY = get_ply->dimensionY;
    float PlayerLimitPositionX = WindowSizeX - PlayerDimensionX;
    float PlayerLimitPositionY = WindowSizeY - PlayerDimensionY;

    float WallPositionX = get_obj->pointX;
    float WallPositionY = get_obj->pointY;
    float WallDimensionX = get_obj->dimensionX;
    float WallDimensionY = get_obj->dimensionY;
   
   
    float DirUp = 0.0f, DirDown = 0.0f, DirLeft = 0.0f, DirRight = 0.0f;
    float const speed = 150.0f;
    float const runSpeed = 2.4f;
    const float invSqrt2 = 0.70710678f;
    float vx = 0.0f, vy = 0.0f, vz = 0.0f;

    float hitBoxPlayer[4][2] = {
        {PlayerPositionX, PlayerPositionY},
        {PlayerPositionX + PlayerDimensionX, PlayerPositionY},
        {PlayerPositionX, PlayerPositionY + PlayerDimensionY},
        {PlayerPositionX + PlayerDimensionX, PlayerPositionY + PlayerDimensionY}
    };
    float hitBoxWall[4][2] = {
        {WallPositionX, WallPositionY},
        {WallPositionX + WallDimensionX, WallPositionY},
        {WallPositionX, WallPositionY + WallDimensionY},
        {WallPositionX + WallDimensionX, WallPositionY + WallDimensionY}
    };
    
    Uint64 FrameStart = SDL_GetPerformanceCounter();
    Uint64 FrameEnd;
    Uint64 FrameTicks;

    while (running) {
        FrameEnd = SDL_GetPerformanceCounter();
        FrameTicks = SDL_GetPerformanceFrequency();
        float dt = (float)(FrameEnd - FrameStart) / (float)FrameTicks;
        FrameStart = FrameEnd;

        // Calculate game state
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) running = false;
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYUP) {
                switch (e.key.keysym.sym) {
                    case SDLK_UP    : DirUp    = 0;
                        break;
                    case SDLK_DOWN  : DirDown  = 0;
                        break;
                    case SDLK_LEFT  : DirLeft  = 0;
                        break;
                    case SDLK_RIGHT : DirRight = 0;
                        break;
                }
            }
            
            if (e.type == SDL_KEYDOWN){
                switch (e.key.keysym.sym) {
                    case SDLK_UP    : DirUp    = -speed;
                        break;
                    case SDLK_DOWN  : DirDown  =  speed;
                        break;
                    case SDLK_LEFT  : DirLeft  = -speed;
                        break;
                    case SDLK_RIGHT : DirRight =  speed;
                        break;
                }
            }
        }

        vx = DirLeft  + DirRight;
        vy = DirUp    + DirDown;
        vz = vx + vy;
        const Uint8 *keys = SDL_GetKeyboardState(NULL);
        
        if (keys[SDL_SCANCODE_LSHIFT] && PlayerCurrentStamina > 0) {
            if (vx != 0 || vy != 0) {
                PlayerCurrentStamina --;
                vx *= runSpeed;
                vy *= runSpeed;
            }
            else if (PlayerCurrentStamina < get_ply->max_st) {
                PlayerCurrentStamina ++;
            }
        }
        else if (PlayerCurrentStamina < get_ply->max_st) {
            if (!keys[SDL_SCANCODE_LSHIFT] || vz == 0) {
                PlayerCurrentStamina ++;
            }
        }

        get_ply->current_st = PlayerCurrentStamina;

        if (vy != 0 && vx != 0) {
            vx = vx * invSqrt2;
            vy = vy * invSqrt2;
        }


        PlayerPositionX += vx * dt;
        PlayerPositionY += vy * dt;


        if ((PlayerPositionX) <= 0 ) PlayerPositionX = 0;
        if ((PlayerPositionX) >= PlayerLimitPositionX) PlayerPositionX = PlayerLimitPositionX;
        if ((PlayerPositionY) <= 0 ) PlayerPositionY = 0;
        if ((PlayerPositionY) >= PlayerLimitPositionY) PlayerPositionY = PlayerLimitPositionY;

        hitBoxPlayer[0][0] = PlayerPositionX;
        hitBoxPlayer[0][1] = PlayerPositionY;
        hitBoxPlayer[1][0] = PlayerPositionX + PlayerDimensionX;
        hitBoxPlayer[1][1] = PlayerPositionY;
        hitBoxPlayer[2][0] = PlayerPositionX;
        hitBoxPlayer[2][1] = PlayerPositionY + PlayerDimensionY;
        hitBoxPlayer[3][0] = PlayerPositionX + PlayerDimensionX;
        hitBoxPlayer[3][1] = PlayerPositionY + PlayerDimensionY;


        if (vx > 0) {
            if (hitBoxPlayer[3][1] <  hitBoxWall[0][1] ||
                hitBoxPlayer[1][1] >  hitBoxWall[2][1]) {
            } else
            if (hitBoxPlayer[1][0] >= hitBoxWall[0][0] &&
                hitBoxPlayer[1][0] <  hitBoxWall[0][0] + vx * dt + 1) {
                PlayerPositionX = hitBoxWall[0][0] - PlayerDimensionX -1;
            }
        }

        if (vx < 0) {
            if (hitBoxPlayer[2][1] <  hitBoxWall[1][1] ||
                hitBoxPlayer[0][1] >  hitBoxWall[3][1]) {
                } else
                if (hitBoxPlayer[0][0] <= hitBoxWall[1][0] &&
                    hitBoxPlayer[0][0] >  hitBoxWall[1][0] + vx * dt - 1) {
                    PlayerPositionX = hitBoxWall[1][0] + 1;
                }
        }

        if (vy > 0) {
            if (hitBoxPlayer[3][0] <  hitBoxWall[0][0] ||
                hitBoxPlayer[2][0] >  hitBoxWall[1][0]) {
            } else
            if (hitBoxPlayer[2][1] >= hitBoxWall[0][1] &&
                hitBoxPlayer[2][1] <  hitBoxWall[0][1] + vy * dt + 1) {
                    PlayerPositionY = hitBoxWall[0][1] - PlayerDimensionY - 1;
            }
        }

        if (vy < 0) {
            if (hitBoxPlayer[1][0] <  hitBoxWall[2][0] ||
                hitBoxPlayer[0][0] >  hitBoxWall[3][0]) {
            } else
            if (hitBoxPlayer[0][1] <= hitBoxWall[2][1] &&
                hitBoxPlayer[0][1] >  hitBoxWall[2][1] + vy * dt - 1) {
                    PlayerPositionY = hitBoxWall[2][1] + 1;
            }
        }


        

        
        // Render





        SDL_SetRenderDrawColor(ren, 120, 80, 80, 255);

        SDL_RenderClear(ren);

        SDL_Rect PlayerRender = { PlayerPositionX, PlayerPositionY, PlayerDimensionX, PlayerDimensionY };
        SDL_SetRenderDrawColor(ren, 255, 104, 230, 255);
        SDL_RenderFillRect(ren, &PlayerRender);
        
        SDL_Rect hp_bar = { 28, 28, PlayerMaxHealth+4, 19 };
        SDL_SetRenderDrawColor(ren, 204, 204, 255, 255);
        SDL_RenderFillRect(ren, &hp_bar);
        
        SDL_Rect hp_empty_bar = { 30, 30, PlayerMaxHealth, 15 };
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &hp_empty_bar);

        SDL_Rect hp = { 30, 30, PlayerCurrentHealth, 15 };
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_RenderFillRect(ren, &hp);

        SDL_Rect st_bar = { 28, 50, PlayerMaxStamina+4, 19 };
        SDL_SetRenderDrawColor(ren, 204, 204, 255, 255);
        SDL_RenderFillRect(ren, &st_bar);

        SDL_Rect st_empty_bar = { 30, 52, PlayerMaxStamina, 15 };
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &st_empty_bar);

        SDL_Rect st = { 30, 52, PlayerCurrentStamina, 15 };
        SDL_SetRenderDrawColor(ren, 102, 255, 51, 255);
        SDL_RenderFillRect(ren, &st);
        
        SDL_Rect wallH = { 30, 500, 200, 10};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &wallH);

        SDL_Rect wallV = { WallPositionX, WallPositionY, WallDimensionX, WallDimensionY};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &wallV);

        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
