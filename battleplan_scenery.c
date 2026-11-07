#include "battleplan_scenery.h"
#include "scenery_paint.h"
#include "window_settings.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#define SCENERY_SEED 0x5eed2026u

#define BORDER_LEFT 120
#define BORDER_RIGHT 120
#define BORDER_TOP 60
#define BORDER_BOTTOM 140

static SDL_Texture *ground = NULL;

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
    vertical_gradient(renderer, scenery_shade(base, 6, 255), scenery_shade(base, -8, 255));

    for (int i = 0; i < 90; i++) {

        const float cx = scenery_noise_range(0.0f, (float)WINDOW_SIZE_X);
        const float cy = scenery_noise_range(0.0f, (float)WINDOW_SIZE_Y);
        const _Bool is_dark = (scenery_noise_next() & 1u) != 0;
        const SDL_Color tone = is_dark ? (SDL_Color){162, 134, 96, 14} : (SDL_Color){236, 218, 180, 24};
        const int blobs = scenery_noise_int(4, 8);
        for (int j = 0; j < blobs; j++) {
            scenery_fill_ellipse(renderer, cx + scenery_noise_range(-70.0f, 70.0f), cy + scenery_noise_range(-40.0f, 40.0f),
                         scenery_noise_range(30.0f, 80.0f), scenery_noise_range(20.0f, 50.0f), tone);
        }

    }

    for (int i = 0; i < 9000; i++) {

        const int size = scenery_noise_int(1, 3);
        const int amount = scenery_noise_int(-34, 22);
        const SDL_Color speck = scenery_shade(base, amount, (Uint8)scenery_noise_int(60, 150));
        SDL_SetRenderDrawColor(renderer, speck.r, speck.g, speck.b, speck.a);
        const SDL_Rect rect = {scenery_noise_int(0, (int)WINDOW_SIZE_X), scenery_noise_int(0, (int)WINDOW_SIZE_Y), size, size};
        SDL_RenderFillRect(renderer, &rect);

    }

    for (int i = 0; i < 260; i++) {

        const float cx = scenery_noise_range(0.0f, (float)WINDOW_SIZE_X);
        const float cy = scenery_noise_range(0.0f, (float)WINDOW_SIZE_Y);
        const float rx = scenery_noise_range(2.0f, 6.0f);
        const float ry = rx * scenery_noise_range(0.6f, 0.9f);
        const SDL_Color stone = scenery_shade((SDL_Color){158, 146, 128, 255}, scenery_noise_int(-24, 18), 255);
        scenery_fill_ellipse(renderer, cx + 1.0f, cy + 1.5f, rx, ry, (SDL_Color){90, 72, 50, 90});
        scenery_fill_ellipse(renderer, cx, cy, rx, ry, stone);
        scenery_fill_ellipse(renderer, cx - rx * 0.3f, cy - ry * 0.35f, rx * 0.45f, ry * 0.4f, scenery_shade(stone, 30, 200));

    }

}

static void tree(SDL_Renderer *renderer, float cx, float cy, float radius) {

    scenery_fill_ellipse(renderer, cx + radius * 0.35f, cy + radius * 0.45f, radius * 1.15f, radius * 0.8f, (SDL_Color){40, 30, 16, 80});
    const int lobes = scenery_noise_int(5, 8);
    for (int i = 0; i < lobes; i++) {
        const float angle = scenery_noise_range(0.0f, 6.2831853f);
        const float reach = scenery_noise_range(0.2f, 0.55f) * radius;
        scenery_fill_circle(renderer, cx + cosf(angle) * reach, cy + sinf(angle) * reach, radius * scenery_noise_range(0.5f, 0.7f),
                    scenery_shade((SDL_Color){44, 72, 38, 255}, scenery_noise_int(-6, 6), 255));
    }
    for (int i = 0; i < lobes; i++) {
        const float angle = scenery_noise_range(3.4f, 5.4f);
        const float reach = scenery_noise_range(0.1f, 0.45f) * radius;
        scenery_fill_circle(renderer, cx + cosf(angle) * reach, cy + sinf(angle) * reach, radius * scenery_noise_range(0.3f, 0.45f),
                    scenery_shade((SDL_Color){66, 102, 52, 255}, scenery_noise_int(-6, 6), 255));
    }
    for (int i = 0; i < lobes / 2 + 1; i++) {
        const float angle = scenery_noise_range(3.6f, 5.0f);
        const float reach = scenery_noise_range(0.2f, 0.5f) * radius;
        scenery_fill_circle(renderer, cx + cosf(angle) * reach, cy + sinf(angle) * reach, radius * scenery_noise_range(0.12f, 0.2f),
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
            for (float along = -40.0f; along < edge_length(edge) + 40.0f; along += scenery_noise_range(18.0f, 34.0f)) {

                float x = 0.0f;
                float y = 0.0f;
                edge_point(edge, along, scenery_noise_range(-0.2f, 0.45f) * depth, &x, &y);
                scenery_fill_circle(renderer, x, y, scenery_noise_range(0.45f, 0.75f) * depth, scenery_shade(layers[layer], scenery_noise_int(-6, 6), 255));

            }

        }

    }

    for (int edge = 0; edge < 4; edge++) {

        const float depth = border_depth(edge);
        for (float along = 0.0f; along < edge_length(edge); along += scenery_noise_range(10.0f, 26.0f)) {

            float x = 0.0f;
            float y = 0.0f;
            edge_point(edge, along, depth * scenery_noise_range(0.85f, 1.35f), &x, &y);
            scenery_grass_tuft(renderer, x, y, scenery_noise_range(8.0f, 15.0f), (SDL_Color){104, 128, 62, 255});

        }

    }

}

static void paint_plants(SDL_Renderer *renderer) {

    for (int edge = 0; edge < 4; edge++) {

        const float depth = border_depth(edge);
        for (float along = scenery_noise_range(0.0f, 80.0f); along < edge_length(edge); along += scenery_noise_range(70.0f, 140.0f)) {

            float x = 0.0f;
            float y = 0.0f;
            edge_point(edge, along, depth * scenery_noise_range(0.25f, 0.8f), &x, &y);
            scenery_bush(renderer, x, y, scenery_noise_range(14.0f, 24.0f));

        }

    }

    for (int edge = 0; edge < 4; edge++) {

        const float depth = border_depth(edge);
        for (float along = scenery_noise_range(0.0f, 120.0f); along < edge_length(edge); along += scenery_noise_range(140.0f, 260.0f)) {

            float x = 0.0f;
            float y = 0.0f;
            edge_point(edge, along, depth * scenery_noise_range(-0.2f, 0.25f), &x, &y);
            tree(renderer, x, y, scenery_noise_range(0.7f, 1.0f) * (depth < 90.0f ? 62.0f : 84.0f));

        }

    }

    const float corners[4][2] = {{0.0f, 0.0f}, {(float)WINDOW_SIZE_X, 0.0f}, {0.0f, (float)WINDOW_SIZE_Y}, {(float)WINDOW_SIZE_X, (float)WINDOW_SIZE_Y}};
    for (int i = 0; i < 4; i++) {
        tree(renderer, corners[i][0] + (corners[i][0] > 0.0f ? -30.0f : 30.0f), corners[i][1] + (corners[i][1] > 0.0f ? -30.0f : 30.0f), 100.0f);
    }

    for (int i = 0; i < 70; i++) {
        const float x = scenery_noise_range((float)BORDER_LEFT + 30.0f, (float)WINDOW_SIZE_X - (float)BORDER_RIGHT - 30.0f);
        const float y = scenery_noise_range((float)BORDER_TOP + 30.0f, (float)WINDOW_SIZE_Y - (float)BORDER_BOTTOM - 30.0f);
        scenery_grass_tuft(renderer, x, y, scenery_noise_range(5.0f, 9.0f), (SDL_Color){128, 140, 76, 255});
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

    scenery_noise_seed(SCENERY_SEED);
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
