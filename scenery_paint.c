#include "scenery_paint.h"
#include <math.h>

#define CIRCLE_SEGMENTS 28

static uint32_t noise_state = 1u;

void scenery_noise_seed(uint32_t seed) {

    noise_state = seed;

}

uint32_t scenery_noise_next(void) {

    noise_state ^= noise_state << 13;
    noise_state ^= noise_state >> 17;
    noise_state ^= noise_state << 5;
    return noise_state;

}

float scenery_noise_range(float low, float high) {

    return low + (high - low) * (float)(scenery_noise_next() & 0xffffffu) / (float)0x1000000u;

}

int scenery_noise_int(int low, int high) {

    return low + (int)(scenery_noise_next() % (uint32_t)(high - low + 1));

}

SDL_Color scenery_shade(SDL_Color color, int amount, Uint8 alpha) {

    const int r = color.r + amount;
    const int g = color.g + amount;
    const int b = color.b + amount;
    return (SDL_Color){
        (Uint8)(r < 0 ? 0 : r > 255 ? 255 : r),
        (Uint8)(g < 0 ? 0 : g > 255 ? 255 : g),
        (Uint8)(b < 0 ? 0 : b > 255 ? 255 : b),
        alpha
    };

}

void scenery_fill_ellipse(SDL_Renderer *renderer, float cx, float cy, float rx, float ry, SDL_Color color) {

    SDL_Vertex vertices[CIRCLE_SEGMENTS + 1];
    int indices[CIRCLE_SEGMENTS * 3];
    vertices[0] = (SDL_Vertex){{cx, cy}, color, {0.0f, 0.0f}};
    for (int i = 0; i < CIRCLE_SEGMENTS; i++) {

        const float angle = (float)i * 6.2831853f / (float)CIRCLE_SEGMENTS;
        vertices[i + 1] = (SDL_Vertex){{cx + cosf(angle) * rx, cy + sinf(angle) * ry}, color, {0.0f, 0.0f}};
        indices[i * 3] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = (i + 1) % CIRCLE_SEGMENTS + 1;

    }
    SDL_RenderGeometry(renderer, NULL, vertices, CIRCLE_SEGMENTS + 1, indices, CIRCLE_SEGMENTS * 3);

}

void scenery_fill_circle(SDL_Renderer *renderer, float cx, float cy, float radius, SDL_Color color) {

    scenery_fill_ellipse(renderer, cx, cy, radius, radius, color);

}

void scenery_grass_tuft(SDL_Renderer *renderer, float x, float y, float height, SDL_Color color) {

    const int blades = scenery_noise_int(3, 5);
    for (int i = 0; i < blades; i++) {

        const float lean = scenery_noise_range(-0.6f, 0.6f) * height;
        const float base = scenery_noise_range(-3.0f, 3.0f);
        const float tall = height * scenery_noise_range(0.7f, 1.1f);
        const SDL_Color blade = scenery_shade(color, scenery_noise_int(-14, 14), 255);
        const SDL_Vertex vertices[3] = {
            {{x + base - 1.6f, y}, blade, {0.0f, 0.0f}},
            {{x + base + 1.6f, y}, blade, {0.0f, 0.0f}},
            {{x + base + lean, y - tall}, scenery_shade(blade, 24, 255), {0.0f, 0.0f}}
        };
        SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);

    }

}

void scenery_bush(SDL_Renderer *renderer, float cx, float cy, float radius) {

    const SDL_Color dark = {58, 88, 44, 255};
    scenery_fill_ellipse(renderer, cx + radius * 0.25f, cy + radius * 0.55f, radius * 1.1f, radius * 0.5f, (SDL_Color){60, 44, 24, 70});
    const int lobes = scenery_noise_int(3, 5);
    for (int i = 0; i < lobes; i++) {
        scenery_fill_circle(renderer, cx + scenery_noise_range(-0.6f, 0.6f) * radius, cy + scenery_noise_range(-0.4f, 0.3f) * radius,
                            radius * scenery_noise_range(0.55f, 0.8f), scenery_shade(dark, scenery_noise_int(-8, 8), 255));
    }
    for (int i = 0; i < lobes; i++) {
        scenery_fill_circle(renderer, cx + scenery_noise_range(-0.5f, 0.3f) * radius, cy + scenery_noise_range(-0.6f, -0.1f) * radius,
                            radius * scenery_noise_range(0.25f, 0.4f), (SDL_Color){92, 128, 62, 255});
    }

}
