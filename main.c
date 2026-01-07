#include <SDL2/SDL.h>
#include <stdbool.h>
#include <math.h>
#include <stdio.h>
#include "objects.h"
#include "player.h"
#include "object_maker.h"

int main(void) {
    
    float MapSizeX, MapSizeY;
    float MapLeftLimit, MapRightLimit, MapTopLimit, MapBottomLimit;
    float WindowSizeX = 800.0f, WindowSizeY = 600.0f;
    MapSizeX = 850.0f;
    MapSizeY = 550.0f;
    
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        SDL_Log("SDL_Init error: %s", SDL_GetError());
        return 1;
    }
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

    int player_id = create_player(50 ,100, "lipe", 100.0f, 100.0f, 30.0f, 30.0f, true, 4);
    
    Player *get_ply = get_player(player_id);

    int PlayerMaxHealth = get_ply->max_hp;
    int PlayerMaxStamina = get_ply->max_st;
    int PlayerCurrentHealth = get_ply->current_hp;
    int PlayerCurrentStamina = get_ply->current_st;
    float PlayerPositionX = get_ply->spawnX;
    float PlayerPositionY = get_ply->spawnY;
    float PlayerDimensionX = get_ply->dimensionX;
    float PlayerDimensionY = get_ply->dimensionY;
    float PlayerLimitPositionX = MapSizeX - PlayerDimensionX;
    float PlayerLimitPositionY = MapSizeY - PlayerDimensionY;
    float PlayerWindowPositionX = WindowSizeX / 2 - PlayerDimensionX / 2;
    float PlayerWindowPositionY = WindowSizeY / 2 - PlayerDimensionY / 2;
    float PlayerPositionHitCornerX = PlayerWindowPositionX;
    float PlayerPositionHitCornerY = PlayerWindowPositionY;
   
    float LX, LY;
    float KX, KY;
    float ZX, ZY;

    MapLeftLimit = 0.0f;
    MapRightLimit = MapSizeX;
    MapTopLimit = 0.0f;
    MapBottomLimit = MapSizeY;
    
    
    
    // Z* moves player spawn to screen center and all objects relatively to player's position
    ZX = PlayerPositionX - WindowSizeX / 2 + PlayerDimensionX / 2;
    ZY = PlayerPositionY - WindowSizeY / 2 + PlayerDimensionY / 2;

    PlayerPositionX -= ZX;
    PlayerPositionY -= ZY;

    MapLeftLimit -= ZX;
    MapRightLimit -= ZX;
    MapTopLimit -= ZY;
    MapBottomLimit -= ZY;

    make_objects();

    int i = 0;
    int x = 0;
    for (i = 0; i < MAX_OBJECTS; i++) {
        Object *obj = get_object(object_id[i]);
        if (obj->id == 0) break;
        x++;
        obj->pointX -= ZX;
        obj->pointY -= ZY;
    }
   
    float DirUp = 0.0f, DirDown = 0.0f, DirLeft = 0.0f, DirRight = 0.0f;
    float const speed = 150.0f;
    float const runSpeed = 2.4f;
    const float invSqrt2 = 0.70710678f;
    float vx = 0.0f, vy = 0.0f, va = 0.0f, vb = 0.0f, vxdt = 0.0f, vydt = 0.0f;

    float hitBoxPlayer[4][2] = {
        {PlayerPositionX, PlayerPositionY},
        {PlayerPositionX + PlayerDimensionX, PlayerPositionY},
        {PlayerPositionX, PlayerPositionY + PlayerDimensionY},
        {PlayerPositionX + PlayerDimensionX, PlayerPositionY + PlayerDimensionY}
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

    float (*hitBoxObject)[4][2] = malloc(x * sizeof *hitBoxMaxObjects);

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

        // this part checks for whether player can still sprint
        vx = DirLeft  + DirRight;
        vy = DirUp    + DirDown;
        va = vx + vy;
        vb = vx * vy;
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
            if (!keys[SDL_SCANCODE_LSHIFT] || va == 0 && vb == 0) {
                PlayerCurrentStamina ++;
            }
        }

        get_ply->current_st = PlayerCurrentStamina;

        // diagonal movement
        if (vy != 0 && vx != 0) {
            vx = vx * invSqrt2;
            vy = vy * invSqrt2;
        }

        vxdt = vx * dt;
        vydt = vy * dt;
        

        // verifies if player hits corner so to stop object from moving
/*
        MapLeftLimit -= vxdt;
        MapRightLimit = MapLeftLimit + MapSizeX;
        MapTopLimit -= vydt;
        MapBottomLimit = MapTopLimit + MapSizeY;
*/
        if (PlayerPositionHitCornerX <= MapLeftLimit - vxdt) {
            LX = MapLeftLimit - PlayerPositionHitCornerX + 1;
            //MapLeftLimit = PlayerPositionHitCornerX - 1;
            vxdt = 0;
            if (LX >= -1) {
                LX = 0;
            }
        }

        if (PlayerPositionHitCornerX + PlayerDimensionX >= MapRightLimit - vxdt) {
            LX = MapRightLimit - PlayerPositionHitCornerX - PlayerDimensionX - 1;
            //MapLeftLimit = -MapSizeX + PlayerPositionHitCornerX + PlayerDimensionX + 1;
            vxdt = 0;
            if (LX <= 1) {
                LX = 0;
            }
        }

        if (PlayerPositionHitCornerY <= MapTopLimit - vydt) {
            LY = MapTopLimit - PlayerPositionHitCornerY + 1;
            //MapTopLimit = PlayerPositionHitCornerY - 1;
            vydt = 0;
            if (LY >= -1) {
                LY = 0;
            }
        }

        if (PlayerPositionHitCornerY + PlayerDimensionY >= MapBottomLimit - vydt) {
            LY = MapBottomLimit - PlayerPositionHitCornerY - PlayerDimensionY - 1;
            //MapTopLimit = -MapSizeY + PlayerPositionHitCornerY + PlayerDimensionY + 1;
            vydt = 0;
            if (LY <= 1) {
                LY = 0;
            }
        }
        /*
        for (int i = 0; i < x; i++) {
            Object *obj = get_object(object_id[i]);
            obj->pointX -= vxdt + LX;
            obj->pointY -= vydt + LY;
        }
*/

/*
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

        WallPositionX -= vxdt + LX;
        WallPositionY -= vydt + LY;
        LX = 0;
        LY = 0;

        hitBoxWall[0][0] = WallPositionX;
        hitBoxWall[0][1] = WallPositionY;
        hitBoxWall[1][0] = WallPositionX + WallDimensionX;
        hitBoxWall[1][1] = WallPositionY;
        hitBoxWall[2][0] = WallPositionX;
        hitBoxWall[2][1] = WallPositionY + WallDimensionY;
        hitBoxWall[3][0] = WallPositionX + WallDimensionX;
        hitBoxWall[3][1] = WallPositionY + WallDimensionY;
*/

        if (vx > 0 && vy > 0) {
            for (int i = 0; i < x; i++) {
               // Object *obj = get_object(object_id[i]);
                if ((hitBoxPlayer[3][1] <  hitBoxObject[i][0][1] -vydt -1 ||
                    hitBoxPlayer[1][1] >  hitBoxObject[i][2][1] -vydt +1) &&
                    (hitBoxPlayer[3][0] <  hitBoxObject[i][0][0] -vxdt -1 ||
                    hitBoxPlayer[2][0] >  hitBoxObject[i][1][0] -vxdt +1)) {
                } else
                if ((hitBoxPlayer[1][0] >= hitBoxObject[i][0][0] -vxdt -1 &&
                    hitBoxPlayer[1][0] <  hitBoxObject[i][0][0] + 1) &&
                    (hitBoxPlayer[2][1] >= hitBoxObject[i][0][1] -vydt -1 &&
                    hitBoxPlayer[2][1] <  hitBoxObject[i][0][1] + 1)) {
                   // obj->pointY = hitBoxPlayer[2][1] - 2;
                    hitBoxObject[i][0][1] -= 3;
                }
            }
        }

        if (vx > 0 && vy < 0) {
            for (int i = 0; i < x; i++) {
              //  Object *obj = get_object(object_id[i]);
                if ((hitBoxPlayer[3][1] <  hitBoxObject[i][0][1] -vydt -1 ||
                    hitBoxPlayer[1][1] >  hitBoxObject[i][2][1] -vydt) +1 &&
                    (hitBoxPlayer[1][0] <  hitBoxObject[i][2][0] -vxdt -1 ||
                    hitBoxPlayer[0][0] >  hitBoxObject[i][3][0] -vxdt +1)) {
                } else
                if ((hitBoxPlayer[1][0] >= hitBoxObject[i][0][0] -vxdt -1 &&
                    hitBoxPlayer[1][0] <  hitBoxObject[i][0][0] + 1) &&
                    (hitBoxPlayer[0][1] <= hitBoxObject[i][2][1] -vydt +1 &&
                    hitBoxPlayer[0][1] >  hitBoxObject[i][2][1] - 1)) {
                    //obj->pointY = hitBoxPlayer[1][1] - obj->pointY + 2;
                    hitBoxObject[i][2][1] += 3;
                }
            }
        }

        if (vx < 0 && vy > 0) {
            for (int i = 0; i < x; i++) {
              //  Object *obj = get_object(object_id[i]);
                if ((hitBoxPlayer[2][1] <  hitBoxObject[i][1][1] -vydt -1 ||
                    hitBoxPlayer[0][1] >  hitBoxObject[i][3][1] -vydt +1) &&
                    (hitBoxPlayer[3][0] <  hitBoxObject[i][0][0] -vxdt -1 ||
                    hitBoxPlayer[2][0] >  hitBoxObject[i][1][0] -vxdt +1)) {
                } else
                if ((hitBoxPlayer[0][0] <= hitBoxObject[i][1][0] -vxdt +1 &&
                    hitBoxPlayer[0][0] >  hitBoxObject[i][1][0] - 1) &&
                    (hitBoxPlayer[2][1] >= hitBoxObject[i][0][1] -vydt -1 &&
                    hitBoxPlayer[2][1] <  hitBoxObject[i][0][1] + 1)) {
                   // obj->pointY = hitBoxPlayer[2][1] - 2;
                    hitBoxObject[i][1][1] -= 3;
                }
            }
        }

        if (vx < 0 && vy < 0) {
            for (int i = 0; i < x; i++) {
             //   Object *obj = get_object(object_id[i]);
                if ((hitBoxPlayer[2][1] <  hitBoxObject[i][1][1] -vydt -1 ||
                    hitBoxPlayer[0][1] >  hitBoxObject[i][3][1] -vydt +1) &&
                    (hitBoxPlayer[1][0] <  hitBoxObject[i][2][0] -vxdt -1 ||
                    hitBoxPlayer[0][0] >  hitBoxObject[i][3][0] -vxdt +1)) {
                } else
                if ((hitBoxPlayer[0][0] <= hitBoxObject[i][1][0] -vxdt +1 &&
                    hitBoxPlayer[0][0] >  hitBoxObject[i][1][0] - 1) &&
                    (hitBoxPlayer[0][1] <= hitBoxObject[i][2][1] -vydt +1 &&
                    hitBoxPlayer[0][1] >  hitBoxObject[i][2][1] - 1)) {
                    //obj->pointY = hitBoxPlayer[0][1] - obj->pointY + 2;
                    hitBoxObject[i][3][1] += 3;
                    vydt += vydt;
                    vxdt -= vxdt;
                }
            }
        }


        if (vx > 0) {
            for (int i = 0; i < x; i++) {
             //   Object *obj = get_object(object_id[i]);
                if (hitBoxPlayer[3][1] <  hitBoxObject[i][0][1] -vydt -1 ||
                    hitBoxPlayer[1][1] >  hitBoxObject[i][2][1] -vydt +1) {
                } else
                if (hitBoxPlayer[1][0] >= hitBoxObject[i][0][0] -vxdt -1 &&
                    hitBoxPlayer[1][0] <  hitBoxObject[i][0][0] + 1) {
                    KX = 0;
                    KX = - hitBoxPlayer[1][0] + hitBoxObject[i][0][0] - 1;
                    vxdt = 0;
                    if (KX < 1) {
                        KX = 0;

                    }
                }     
            }       
        }

        if (vx < 0) {
            for (int i = 0; i < x; i++) {
              //  Object *obj = get_object(object_id[i]);
                if (hitBoxPlayer[2][1] <  hitBoxObject[i][1][1] -vydt -1 ||
                    hitBoxPlayer[0][1] >  hitBoxObject[i][3][1] -vydt +1) {
                } else
                if (hitBoxPlayer[0][0] <= hitBoxObject[i][1][0] -vxdt +1 &&
                    hitBoxPlayer[0][0] >  hitBoxObject[i][1][0] - 1) {
                 //   obj->pointX = hitBoxPlayer[0][0] - obj->dimensionX - 1;
                   // MapLeftLimit += hitBoxPlayer[0][0] - hitBoxObject[i][1][0] - 1;
                    KX = - hitBoxPlayer[0][0] + hitBoxObject[i][1][0] + 1;
                    vxdt = 0;
                    if (KX > -1) {
                        KX = 0;
                    }
                }
            }
        }

        if (vy > 0) {
            for (int i = 0; i < x; i++) {
               // Object *obj = get_object(object_id[i]);
                if (hitBoxPlayer[3][0] <  hitBoxObject[i][0][0] -vxdt -1 ||
                    hitBoxPlayer[2][0] >  hitBoxObject[i][1][0] -vxdt +1) {
                } else
                if (hitBoxPlayer[2][1] >= hitBoxObject[i][0][1] -vydt -1 &&
                    hitBoxPlayer[2][1] <  hitBoxObject[i][0][1] + 1) {
                    //obj->pointY = hitBoxPlayer[2][1] + 1;
                    //MapTopLimit += hitBoxPlayer[2][1] - hitBoxObject[i][0][1] + 1;
                    KY = - hitBoxPlayer[2][1] + hitBoxObject[i][0][1] - 1;
                    vydt = 0;
                    if (KY < 1) {
                        KY = 0;
                    }
                }
            }
        }

        if (vy < 0) {
            for (int i = 0; i < x; i++) {
              //  Object *obj = get_object(object_id[i]);
                if (hitBoxPlayer[1][0] <  hitBoxObject[i][2][0] -vxdt -1 ||
                    hitBoxPlayer[0][0] >  hitBoxObject[i][3][0] -vxdt +1) {
                } else
                if (hitBoxPlayer[0][1] <= hitBoxObject[i][2][1] -vydt +1 &&
                    hitBoxPlayer[0][1] >  hitBoxObject[i][2][1] - 1) {
                    //obj->pointY = hitBoxPlayer[1][1] - obj->dimensionY - 1;
                   // MapTopLimit += hitBoxPlayer[0][1] - hitBoxObject[i][2][1] - 1;
                    KY = - hitBoxPlayer[0][1] + hitBoxObject[i][2][1] + 1;
                    vydt = 0;
                    if (KY > -1) {
                        KY = 0;
                    }
                }
            }
        }




        for (int i = 0; i < x; i++) {
            Object *obj = get_object(object_id[i]);
            obj->pointX -= vxdt + LX + KX;
            obj->pointY -= vydt + LY + KY;
        }



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

        MapLeftLimit -= vxdt + LX + KX;
        MapRightLimit = MapLeftLimit + MapSizeX;
        MapTopLimit -= vydt + LY + KY;
        MapBottomLimit = MapTopLimit + MapSizeY;



        KX = 0;
        KY = 0;
        LX = 0;
        LY = 0;


        // Render





        SDL_SetRenderDrawColor(ren, 120, 80, 80, 255);

        SDL_RenderClear(ren);

        SDL_Rect Map = {MapLeftLimit, MapTopLimit, MapSizeX, MapSizeY};
        SDL_SetRenderDrawColor(ren, 0, 0, 0, 255);
        SDL_RenderFillRect(ren, &Map);

        SDL_Rect PlayerRender = { PlayerWindowPositionX, PlayerWindowPositionY, PlayerDimensionX, PlayerDimensionY };
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
        

        for (i = 0; i < x; i++) {
            Object *obj = get_object(object_id[i]);
            if (obj->id == 0) break;
            SDL_Rect Object_Render = {obj->pointX, obj->pointY, obj->dimensionX, obj->dimensionY};
            SDL_SetRenderDrawColor(ren, obj->R_Color, obj->G_Color, obj->B_Color, obj->Alpha);
            SDL_RenderFillRect(ren, &Object_Render);

        }


        SDL_RenderPresent(ren);
    }

    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
