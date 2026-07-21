#include "camera.h"
#include "camera_internal.h"
#include "engine_internal.h"
#include "game_state_internal.h"
#include "player_internal.h"
#include <SDL2/SDL.h>

#define MIN_ZOOM 0.25f
#define MAX_ZOOM 4.0f
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

    camera.position_x = engine.player->playerPositionY;
    camera.position_y = engine.player->playerPositionY;
    camera.zoom = clampf(camera.zoom, MIN_ZOOM, MAX_ZOOM);

}

void camera_world_to_screen(
    float world_pos_x, float world_pos_y,
    float *screen_pos_x, float *screen_pos_y) {
    
    *screen_pos_x = ((world_pos_x - camera.position_x) * camera.zoom) - engine.game->offSetX - 10 +engine.player->playerPositionX;
    *screen_pos_y = ((world_pos_y - camera.position_y) * camera.zoom) - engine.game->offSetY - 10 +engine.player->playerPositionY;

}

void camera_render(void) { //mudar

}

void camera_destroy(void) {

}