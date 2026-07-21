#ifndef CAMERA_H
#define CAMERA_H

#include <SDL2/SDL.h>

void camera_init(void);

void camera_input(SDL_Event *e);

float clampf(float value, float min, float max);

void camera_update(void);

void camera_render(void);

void camera_world_to_screen(
    float world_pos_x, float world_pos_y,
    float *screen_pos_x, float *screen_pos_y);

void camera_destroy(void);

#endif