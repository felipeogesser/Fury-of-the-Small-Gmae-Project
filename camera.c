#include "camera.h"
#include "camera_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "player_internal.h"
#include "window_settings.h"
#include <SDL2/SDL.h>

#define MIN_ZOOM 0.25f
#define MAX_ZOOM 10.0f
#define ZOOM_SPEED 0.1f 

Camera camera = {0};

void camera_init(void) {

    engine.camera = &camera;
    camera.zoom = 1.0f;

}

void camera_input(SDL_Event *e) {

    /*if (e->type == SDL_KEYUP) {
        switch (e->key.keysym.sym) {
            case SDLK_UP    : camera->dir_up    = 0;
                break;
            case SDLK_DOWN  : camera->dir_down  = 0;
                break;
            case SDLK_LEFT  : camera->dir_left  = 0;
                break;
            case SDLK_RIGHT : camera->dir_right = 0;
                break;
        }
    }
    
    if (e->type == SDL_KEYDOWN) {
        switch (e->key.keysym.sym) {
            case SDLK_UP    : camera->dir_up    = -camera->speed;
                break;
            case SDLK_DOWN  : camera->dir_down  =  camera->speed;
                break;
            case SDLK_LEFT  : camera->dir_left  = -camera->speed;
                break;
            case SDLK_RIGHT : camera->dir_right =  camera->speed;
                break;
        }
    }*/

    if (e->type == SDL_MOUSEWHEEL) {
        signed int y = e->wheel.y;
        y = (e->wheel.direction == SDL_MOUSEWHEEL_FLIPPED) ? -y : y;
        camera.zoom += (float)y * ZOOM_SPEED;
    }

}

float clampf(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

void camera_update(void) {

    camera.position_x = engine.player->playerPositionX + engine.player->playerDimensionX / 2.0f;
    camera.position_y = engine.player->playerPositionY + engine.player->playerDimensionY / 2.0f;
    camera.zoom = clampf(camera.zoom, MIN_ZOOM, MAX_ZOOM);

}

void camera_world_to_screen(SDL_Rect *sdl_rect) {
    
    sdl_rect->x = (signed int)(((float)sdl_rect->x - camera.position_x) * camera.zoom + WINDOW_SIZE_X / 2.0f);
    sdl_rect->y = (signed int)(((float)sdl_rect->y - camera.position_y) * camera.zoom + WINDOW_SIZE_Y / 2.0f);
    sdl_rect->w = (signed int)((float)sdl_rect->w * camera.zoom);
    sdl_rect->h = (signed int)((float)sdl_rect->h * camera.zoom);

}

void camera_render(void) { //mudar

}

void camera_destroy(void) {

}