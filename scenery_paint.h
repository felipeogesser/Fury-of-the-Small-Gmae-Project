#ifndef SCENERY_PAINT_H
#define SCENERY_PAINT_H

#include <SDL2/SDL.h>
#include <stdint.h>

void scenery_noise_seed(uint32_t seed);

uint32_t scenery_noise_next(void);

float scenery_noise_range(float low, float high);

int scenery_noise_int(int low, int high);

SDL_Color scenery_shade(SDL_Color color, int amount, Uint8 alpha);

void scenery_fill_ellipse(SDL_Renderer *renderer, float cx, float cy, float rx, float ry, SDL_Color color);

void scenery_fill_circle(SDL_Renderer *renderer, float cx, float cy, float radius, SDL_Color color);

void scenery_grass_tuft(SDL_Renderer *renderer, float x, float y, float height, SDL_Color color);

void scenery_bush(SDL_Renderer *renderer, float cx, float cy, float radius);

#endif
