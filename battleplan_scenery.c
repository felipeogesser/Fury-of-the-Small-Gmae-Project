#include "battleplan_scenery.h"
#include "window_settings.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define SCENERY_SEED 0x5eed2026u
#define CIRCLE_SEGMENTS 28

#define BORDER_LEFT 120
#define BORDER_RIGHT 120
#define BORDER_TOP 60
#define BORDER_BOTTOM 140

static SDL_Texture *ground = NULL;
static uint32_t noise_state = SCENERY_SEED;

static uint32_t noise_next(void) {

    noise_state ^= noise_state << 13;
    noise_state ^= noise_state >> 17;
    noise_state ^= noise_state << 5;
    return noise_state;

}

static float noise_range(float low, float high) {

    return low + (high - low) * (float)(noise_next() & 0xffffffu) / (float)0x1000000u;

}

static int noise_int(int low, int high) {

    return low + (int)(noise_next() % (uint32_t)(high - low + 1));

}

static SDL_Color shade(SDL_Color color, int amount, Uint8 alpha) {

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

static void fill_ellipse(SDL_Renderer *renderer, float cx, float cy, float rx, float ry, SDL_Color color) {

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

static void fill_circle(SDL_Renderer *renderer, float cx, float cy, float radius, SDL_Color color) {

    fill_ellipse(renderer, cx, cy, radius, radius, color);

}

static void vertical_gradient(SDL_Renderer *renderer, SDL_Color top, SDL_Color bottom) {

    const float w = (float)WINDOW_SIZE_X;
    const float h = (float)WINDOW_SIZE_Y;
    const SDL_Vertex vertices[4] = {
        {{0.0f, 0.0f}, top, {0.0f, 0.0f}},
        {{w, 0.0f}, top, {0.0f, 0.0f}},
        {{w, h}, bottom, {0.0f, 0.0f}},
        {{0.0f, h}, bottom, {0.0f, 0.0f}}
    };
    const int indices[6] = {0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);

}

static void paint_dirt(SDL_Renderer *renderer) {

    const SDL_Color base = {204, 180, 138, 255};
    vertical_gradient(renderer, shade(base, 6, 255), shade(base, -8, 255));

    for (int i = 0; i < 90; i++) {

        const float cx = noise_range(0.0f, (float)WINDOW_SIZE_X);
        const float cy = noise_range(0.0f, (float)WINDOW_SIZE_Y);
        const _Bool is_dark = (noise_next() & 1u) != 0;
        const SDL_Color tone = is_dark ? (SDL_Color){162, 134, 96, 14} : (SDL_Color){236, 218, 180, 24};
        const int blobs = noise_int(4, 8);
        for (int j = 0; j < blobs; j++) {
            fill_ellipse(renderer, cx + noise_range(-70.0f, 70.0f), cy + noise_range(-40.0f, 40.0f),
                         noise_range(30.0f, 80.0f), noise_range(20.0f, 50.0f), tone);
        }

    }

    for (int i = 0; i < 9000; i++) {

        const int size = noise_int(1, 3);
        const int amount = noise_int(-34, 22);
        const SDL_Color speck = shade(base, amount, (Uint8)noise_int(60, 150));
        SDL_SetRenderDrawColor(renderer, speck.r, speck.g, speck.b, speck.a);
        const SDL_Rect rect = {noise_int(0, (int)WINDOW_SIZE_X), noise_int(0, (int)WINDOW_SIZE_Y), size, size};
        SDL_RenderFillRect(renderer, &rect);

    }

    for (int i = 0; i < 260; i++) {

        const float cx = noise_range(0.0f, (float)WINDOW_SIZE_X);
        const float cy = noise_range(0.0f, (float)WINDOW_SIZE_Y);
        const float rx = noise_range(2.0f, 6.0f);
        const float ry = rx * noise_range(0.6f, 0.9f);
        const SDL_Color stone = shade((SDL_Color){158, 146, 128, 255}, noise_int(-24, 18), 255);
        fill_ellipse(renderer, cx + 1.0f, cy + 1.5f, rx, ry, (SDL_Color){90, 72, 50, 90});
        fill_ellipse(renderer, cx, cy, rx, ry, stone);
        fill_ellipse(renderer, cx - rx * 0.3f, cy - ry * 0.35f, rx * 0.45f, ry * 0.4f, shade(stone, 30, 200));

    }

}

static void grass_tuft(SDL_Renderer *renderer, float x, float y, float height, SDL_Color color) {

    const int blades = noise_int(3, 5);
    for (int i = 0; i < blades; i++) {

        const float lean = noise_range(-0.6f, 0.6f) * height;
        const float base = noise_range(-3.0f, 3.0f);
        const float tall = height * noise_range(0.7f, 1.1f);
        const SDL_Color blade = shade(color, noise_int(-14, 14), 255);
        const SDL_Vertex vertices[3] = {
            {{x + base - 1.6f, y}, blade, {0.0f, 0.0f}},
            {{x + base + 1.6f, y}, blade, {0.0f, 0.0f}},
            {{x + base + lean, y - tall}, shade(blade, 24, 255), {0.0f, 0.0f}}
        };
        SDL_RenderGeometry(renderer, NULL, vertices, 3, NULL, 0);

    }

}

static void bush(SDL_Renderer *renderer, float cx, float cy, float radius) {

    const SDL_Color dark = {58, 88, 44, 255};
    fill_ellipse(renderer, cx + radius * 0.25f, cy + radius * 0.55f, radius * 1.1f, radius * 0.5f, (SDL_Color){60, 44, 24, 70});
    const int lobes = noise_int(3, 5);
    for (int i = 0; i < lobes; i++) {
        fill_circle(renderer, cx + noise_range(-0.6f, 0.6f) * radius, cy + noise_range(-0.4f, 0.3f) * radius,
                    radius * noise_range(0.55f, 0.8f), shade(dark, noise_int(-8, 8), 255));
    }
    for (int i = 0; i < lobes; i++) {
        fill_circle(renderer, cx + noise_range(-0.5f, 0.3f) * radius, cy + noise_range(-0.6f, -0.1f) * radius,
                    radius * noise_range(0.25f, 0.4f), (SDL_Color){92, 128, 62, 255});
    }

}

static void tree(SDL_Renderer *renderer, float cx, float cy, float radius) {

    fill_ellipse(renderer, cx + radius * 0.35f, cy + radius * 0.45f, radius * 1.15f, radius * 0.8f, (SDL_Color){40, 30, 16, 80});
    const int lobes = noise_int(5, 8);
    for (int i = 0; i < lobes; i++) {
        const float angle = noise_range(0.0f, 6.2831853f);
        const float reach = noise_range(0.2f, 0.55f) * radius;
        fill_circle(renderer, cx + cosf(angle) * reach, cy + sinf(angle) * reach, radius * noise_range(0.5f, 0.7f),
                    shade((SDL_Color){44, 72, 38, 255}, noise_int(-6, 6), 255));
    }
    for (int i = 0; i < lobes; i++) {
        const float angle = noise_range(3.4f, 5.4f);
        const float reach = noise_range(0.1f, 0.45f) * radius;
        fill_circle(renderer, cx + cosf(angle) * reach, cy + sinf(angle) * reach, radius * noise_range(0.3f, 0.45f),
                    shade((SDL_Color){66, 102, 52, 255}, noise_int(-6, 6), 255));
    }
    for (int i = 0; i < lobes / 2 + 1; i++) {
        const float angle = noise_range(3.6f, 5.0f);
        const float reach = noise_range(0.2f, 0.5f) * radius;
        fill_circle(renderer, cx + cosf(angle) * reach, cy + sinf(angle) * reach, radius * noise_range(0.12f, 0.2f),
                    (SDL_Color){104, 140, 72, 255});
    }

}

static float border_depth(int edge) {

    if (edge == 0) return (float)BORDER_TOP;
    if (edge == 1) return (float)BORDER_RIGHT;
    if (edge == 2) return (float)BORDER_BOTTOM;
    return (float)BORDER_LEFT;

}

static void edge_point(int edge, float along, float inward, float *x, float *y) {

    const float w = (float)WINDOW_SIZE_X;
    const float h = (float)WINDOW_SIZE_Y;
    if (edge == 0) { *x = along; *y = inward; return; }
    if (edge == 1) { *x = w - inward; *y = along; return; }
    if (edge == 2) { *x = along; *y = h - inward; return; }
    *x = inward;
    *y = along;

}

static float edge_length(int edge) {

    return (edge == 0 || edge == 2) ? (float)WINDOW_SIZE_X : (float)WINDOW_SIZE_Y;

}

static void paint_meadow(SDL_Renderer *renderer) {

    const SDL_Color layers[3] = {{120, 140, 78, 255}, {104, 128, 66, 255}, {88, 114, 56, 255}};
    for (int layer = 0; layer < 3; layer++) {

        for (int edge = 0; edge < 4; edge++) {

            const float depth = border_depth(edge) * (1.0f - 0.28f * (float)layer);
            for (float along = -40.0f; along < edge_length(edge) + 40.0f; along += noise_range(18.0f, 34.0f)) {

                float x = 0.0f;
                float y = 0.0f;
                edge_point(edge, along, noise_range(-0.2f, 0.45f) * depth, &x, &y);
                fill_circle(renderer, x, y, noise_range(0.45f, 0.75f) * depth, shade(layers[layer], noise_int(-6, 6), 255));

            }

        }

    }

    for (int edge = 0; edge < 4; edge++) {

        const float depth = border_depth(edge);
        for (float along = 0.0f; along < edge_length(edge); along += noise_range(10.0f, 26.0f)) {

            float x = 0.0f;
            float y = 0.0f;
            edge_point(edge, along, depth * noise_range(0.85f, 1.35f), &x, &y);
            grass_tuft(renderer, x, y, noise_range(8.0f, 15.0f), (SDL_Color){104, 128, 62, 255});

        }

    }

}

static void paint_plants(SDL_Renderer *renderer) {

    for (int edge = 0; edge < 4; edge++) {

        const float depth = border_depth(edge);
        for (float along = noise_range(0.0f, 80.0f); along < edge_length(edge); along += noise_range(70.0f, 140.0f)) {

            float x = 0.0f;
            float y = 0.0f;
            edge_point(edge, along, depth * noise_range(0.25f, 0.8f), &x, &y);
            bush(renderer, x, y, noise_range(14.0f, 24.0f));

        }

    }

    for (int edge = 0; edge < 4; edge++) {

        const float depth = border_depth(edge);
        for (float along = noise_range(0.0f, 120.0f); along < edge_length(edge); along += noise_range(140.0f, 260.0f)) {

            float x = 0.0f;
            float y = 0.0f;
            edge_point(edge, along, depth * noise_range(-0.2f, 0.25f), &x, &y);
            tree(renderer, x, y, noise_range(0.7f, 1.0f) * (depth < 90.0f ? 62.0f : 84.0f));

        }

    }

    const float corners[4][2] = {{0.0f, 0.0f}, {(float)WINDOW_SIZE_X, 0.0f}, {0.0f, (float)WINDOW_SIZE_Y}, {(float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y}};
    for (int i = 0; i < 4; i++) {
        tree(renderer, corners[i][0] + (corners[i][0] > 0.0f ? -30.0f : 30.0f), corners[i][1] + (corners[i][1] > 0.0f ? -30.0f : 30.0f), 100.0f);
    }

    for (int i = 0; i < 70; i++) {
        const float x = noise_range((float)BORDER_LEFT + 30.0f, (float)WINDOW_SIZE_X - (float)BORDER_RIGHT - 30.0f);
        const float y = noise_range((float)BORDER_TOP + 30.0f, (float)WINDOW_SIZE_Y - (float)BORDER_BOTTOM - 30.0f);
        grass_tuft(renderer, x, y, noise_range(5.0f, 9.0f), (SDL_Color){128, 140, 76, 255});
    }

}

void battleplan_scenery_init(SDL_Renderer *renderer) {

    if (ground != NULL) return;

    ground = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, (int)WINDOW_SIZE_X, (int)WINDOW_SIZE_Y);
    if (ground == NULL) {
        SDL_Log("battleplan scenery texture failed: %s", SDL_GetError());
        return;
    }

    SDL_Texture *previous = SDL_GetRenderTarget(renderer);
    SDL_BlendMode previous_blend = SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(renderer, &previous_blend);

    SDL_SetRenderTarget(renderer, ground);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    noise_state = SCENERY_SEED;
    paint_dirt(renderer);
    paint_meadow(renderer);
    paint_plants(renderer);

    SDL_SetRenderTarget(renderer, previous);
    SDL_SetRenderDrawBlendMode(renderer, previous_blend);

}

_Bool battleplan_scenery_render(SDL_Renderer *renderer) {

    if (ground == NULL) return false;
    SDL_RenderCopy(renderer, ground, NULL, NULL);
    return true;

}

void battleplan_scenery_destroy(void) {

    if (ground != NULL) SDL_DestroyTexture(ground);
    ground = NULL;

}
