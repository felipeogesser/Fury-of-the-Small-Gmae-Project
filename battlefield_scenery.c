#include "battlefield_scenery.h"
#include "camera.h"
#include "camera_internal.h"
#include "engine_internal.h"
#include "maps_internal.h"
#include "scenery_paint.h"
#include "window_settings.h"
#include <math.h>
#include <stdbool.h>

#define FOREST_SEED 0x91ee5eedu

#define VIEW_MARGIN_X 3700.0f
#define VIEW_MARGIN_Y 2100.0f
#define GROUND_WORLD_PER_TEXEL 4.0f
#define GROUND_FEATHER_TEXELS 1.6f
#define GROUND_SEGMENTS 28
#define DETAIL_TILE_SIZE 1536

#define PINE_VARIANTS 8
#define PINE_TEXTURE_W 256
#define PINE_TEXTURE_H 512
#define PINE_WORLD_W 128.0f
#define PINE_WORLD_H 256.0f
#define PINE_MIN_SCALE 0.75f
#define PINE_MAX_SCALE 1.3f
#define RAGGED_STEP 10.0f
#define RAGGED_MAX_POINTS 64

#define BUSH_VARIANTS 3
#define BUSH_TEXTURE_W 128
#define BUSH_TEXTURE_H 96
#define TUFT_VARIANTS 3
#define TUFT_TEXTURE_W 192
#define TUFT_TEXTURE_H 64
#define SHADOW_TEXTURE_W 128
#define SHADOW_TEXTURE_H 48

#define NEAR_FOREST_DEPTH 1000.0f
#define FOREGROUND_SIDE_DEPTH 750.0f
#define FOREGROUND_BOTTOM_DEPTH 620.0f
#define BACKGROUND_TREE_INTRUSION 110.0f
#define FOREGROUND_INTRUSION 40.0f
#define UNDERGROWTH_INSIDE_REACH 90.0f
#define UNDERGROWTH_OUTSIDE_REACH 620.0f

#define MAX_BACKGROUND_INSTANCES 8192
#define MAX_FOREGROUND_INSTANCES 2048

typedef struct SceneryInstance {

    SDL_Texture *texture;
    SDL_Rect rect;
    Uint8 tint;
    _Bool casts_shadow;

} SceneryInstance;

typedef struct ForestBounds {

    float left, right, top, bottom;

} ForestBounds;

static SDL_Texture *ground = NULL;
static SDL_Texture *detail_tile = NULL;
static SDL_Texture *shadow = NULL;
static SDL_Texture *pines[PINE_VARIANTS] = {NULL};
static SDL_Texture *bushes[BUSH_VARIANTS] = {NULL};
static SDL_Texture *tufts[TUFT_VARIANTS] = {NULL};
static SceneryInstance background[MAX_BACKGROUND_INSTANCES];
static SceneryInstance foreground[MAX_FOREGROUND_INSTANCES];
static unsigned int background_count = 0;
static unsigned int foreground_count = 0;
static SDL_Rect ground_world = {0, 0, 0, 0};
static SDL_Rect map_world = {0, 0, 0, 0};

static float maxf(float a, float b) {

    return a > b ? a : b;

}

static float minf(float a, float b) {

    return a < b ? a : b;

}

static SDL_Color lerp_color(SDL_Color a, SDL_Color b, float t) {

    return (SDL_Color){
        (Uint8)((float)a.r + ((float)b.r - (float)a.r) * t),
        (Uint8)((float)a.g + ((float)b.g - (float)a.g) * t),
        (Uint8)((float)a.b + ((float)b.b - (float)a.b) * t),
        255
    };

}

static SDL_Texture *create_layer(SDL_Renderer *renderer, int width, int height, SDL_Color clear) {

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, width, height);
    if (texture == NULL) {
        SDL_Log("battlefield scenery texture failed: %s", SDL_GetError());
        return NULL;
    }

    SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    SDL_SetTextureScaleMode(texture, SDL_ScaleModeLinear);
    SDL_SetRenderTarget(renderer, texture);
    SDL_SetRenderDrawColor(renderer, clear.r, clear.g, clear.b, clear.a);
    SDL_RenderClear(renderer);
    return texture;

}

static void fill_quad(SDL_Renderer *renderer, const SDL_FPoint points[4], SDL_Color color) {

    const SDL_Vertex vertices[4] = {
        {points[0], color, {0.0f, 0.0f}},
        {points[1], color, {0.0f, 0.0f}},
        {points[2], color, {0.0f, 0.0f}},
        {points[3], color, {0.0f, 0.0f}}
    };
    const int indices[6] = {0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);

}

static void paint_trunk(SDL_Renderer *renderer, float cx, float ground_y, float top_y, float half_width) {

    const SDL_Color bark = {78, 56, 38, 255};
    const float top_half = half_width * 0.45f;
    const SDL_FPoint body[4] = {
        {cx - half_width, ground_y},
        {cx - top_half, top_y},
        {cx + top_half, top_y},
        {cx + half_width, ground_y}
    };
    fill_quad(renderer, body, bark);

    const SDL_FPoint lit[4] = {
        {cx - half_width, ground_y},
        {cx - top_half, top_y},
        {cx - top_half * 0.35f, top_y},
        {cx - half_width * 0.35f, ground_y}
    };
    fill_quad(renderer, lit, scenery_shade(bark, 24, 255));

    const SDL_FPoint dim[4] = {
        {cx + half_width * 0.3f, ground_y},
        {cx + top_half * 0.3f, top_y},
        {cx + top_half, top_y},
        {cx + half_width, ground_y}
    };
    fill_quad(renderer, dim, scenery_shade(bark, -26, 255));

    const float height = ground_y - top_y;
    const int notches = (int)(height / 7.0f);
    for (int i = 0; i < notches; i++) {

        const float t = scenery_noise_range(0.0f, 1.0f);
        const float span = half_width + (top_half - half_width) * t;
        const float x = cx + scenery_noise_range(-0.8f, 0.6f) * span;
        const float y = ground_y - t * height;
        const SDL_Rect notch = {(int)x, (int)y, scenery_noise_int(2, 5), scenery_noise_int(1, 2)};
        const SDL_Color tone = scenery_shade(bark, scenery_noise_int(-34, -14), 255);
        SDL_SetRenderDrawColor(renderer, tone.r, tone.g, tone.b, tone.a);
        SDL_RenderFillRect(renderer, &notch);

    }

    const SDL_Color root = scenery_shade(bark, -10, 255);
    const SDL_Vertex roots[6] = {
        {{cx - half_width * 2.0f, ground_y + 2.0f}, root, {0.0f, 0.0f}},
        {{cx - half_width * 0.6f, ground_y - height * 0.16f}, root, {0.0f, 0.0f}},
        {{cx - half_width * 0.4f, ground_y + 2.0f}, root, {0.0f, 0.0f}},
        {{cx + half_width * 2.0f, ground_y + 2.0f}, root, {0.0f, 0.0f}},
        {{cx + half_width * 0.6f, ground_y - height * 0.16f}, root, {0.0f, 0.0f}},
        {{cx + half_width * 0.4f, ground_y + 2.0f}, root, {0.0f, 0.0f}}
    };
    SDL_RenderGeometry(renderer, NULL, roots, 6, NULL, 0);

}

static void ragged_fan(SDL_Renderer *renderer, float cx, float tip_y, float base_y, float half_width, float hang, SDL_Color tip, SDL_Color lit, SDL_Color dim) {

    SDL_Vertex vertices[RAGGED_MAX_POINTS + 1];
    int indices[(RAGGED_MAX_POINTS - 1) * 3];

    int points = (int)(half_width * 2.0f / RAGGED_STEP) + 3;
    if (points > RAGGED_MAX_POINTS) points = RAGGED_MAX_POINTS;

    vertices[0] = (SDL_Vertex){{cx, tip_y}, tip, {0.0f, 0.0f}};
    for (int i = 0; i < points; i++) {

        const float t = (float)i / (float)(points - 1);
        const float side = t * 2.0f - 1.0f;
        const float arch = (1.0f - side * side) * half_width * 0.14f;
        const float jitter = RAGGED_STEP * scenery_noise_range(-0.3f, 0.3f);
        const float drop = (i % 2 == 1) ? scenery_noise_range(hang * 0.45f, hang) : -scenery_noise_range(0.0f, hang * 0.3f);
        vertices[i + 1] = (SDL_Vertex){{cx + side * half_width + jitter, base_y - arch + drop}, lerp_color(lit, dim, t), {0.0f, 0.0f}};

    }

    for (int i = 0; i < points - 1; i++) {

        indices[i * 3] = 0;
        indices[i * 3 + 1] = i + 1;
        indices[i * 3 + 2] = i + 2;

    }

    SDL_RenderGeometry(renderer, NULL, vertices, points + 1, indices, (points - 1) * 3);

}

static void paint_needles(SDL_Renderer *renderer, float cx, float tip_y, float base_y, float half_width, float hang, SDL_Color lit, SDL_Color dim) {

    const int edge_blades = (int)(half_width / 2.2f);
    for (int i = 0; i < edge_blades; i++) {

        const float side = scenery_noise_range(-1.0f, 1.0f);
        const float t = (side + 1.0f) * 0.5f;
        const float arch = (1.0f - side * side) * half_width * 0.14f;
        const float x = cx + side * half_width;
        const float y = base_y - arch;
        const SDL_Color color = scenery_shade(lerp_color(lit, dim, t), scenery_noise_int(-10, 12), 255);
        const SDL_Vertex blade[3] = {
            {{x - 2.2f, y - 5.0f}, color, {0.0f, 0.0f}},
            {{x + 2.2f, y - 5.0f}, color, {0.0f, 0.0f}},
            {{x + side * 5.0f + scenery_noise_range(-3.0f, 3.0f), y + scenery_noise_range(hang * 0.4f, hang * 1.05f)}, scenery_shade(color, -14, 255), {0.0f, 0.0f}}
        };
        SDL_RenderGeometry(renderer, NULL, blade, 3, NULL, 0);

    }

    const int inner_blades = (int)(half_width / 1.6f);
    for (int i = 0; i < inner_blades; i++) {

        const float along = scenery_noise_range(0.3f, 0.95f);
        const float side = scenery_noise_range(-0.92f, 0.92f);
        const float x = cx + side * half_width * along;
        const float y = tip_y + (base_y - tip_y) * along;
        const float t = (side + 1.0f) * 0.5f;
        const int amount = t < 0.5f ? scenery_noise_int(6, 26) : scenery_noise_int(-18, 4);
        const SDL_Color color = scenery_shade(lerp_color(lit, dim, t), amount, 255);
        const float length = scenery_noise_range(6.0f, 14.0f);
        const SDL_Vertex blade[3] = {
            {{x - 1.5f, y - length * 0.3f}, color, {0.0f, 0.0f}},
            {{x + 1.5f, y - length * 0.3f}, color, {0.0f, 0.0f}},
            {{x + side * length * 0.8f, y + length * 0.7f}, color, {0.0f, 0.0f}}
        };
        SDL_RenderGeometry(renderer, NULL, blade, 3, NULL, 0);

    }

}

static void paint_pine(SDL_Renderer *renderer, SDL_Color needle) {

    const float width = (float)PINE_TEXTURE_W;
    const float height = (float)PINE_TEXTURE_H;
    const float cx = width * 0.5f + scenery_noise_range(-4.0f, 4.0f);
    const float ground_y = height - 8.0f;
    const float trunk_visible = height * scenery_noise_range(0.15f, 0.21f);
    const float foliage_bottom = ground_y - trunk_visible;
    const float foliage_top = 6.0f;
    const float span = foliage_bottom - foliage_top;
    const float max_half_width = width * scenery_noise_range(0.38f, 0.46f);
    const int tiers = scenery_noise_int(5, 7);

    paint_trunk(renderer, cx, ground_y, foliage_bottom - span * 0.3f, width * 0.042f);

    const SDL_Color under = scenery_shade(needle, -24, 255);
    for (int tier = 0; tier < tiers; tier++) {

        const float t = (float)tier / (float)(tiers - 1);
        const float base_y = foliage_bottom - span * 0.78f * t;
        const float tip_y = base_y - span * (0.34f - 0.12f * t);
        const float half_width = max_half_width * (1.0f - t * 0.8f) * scenery_noise_range(0.93f, 1.05f);
        const float tier_cx = cx + scenery_noise_range(-2.5f, 2.5f);
        const float hang = 10.0f + 16.0f * (1.0f - t);
        const int tone = scenery_noise_int(-5, 5);
        const SDL_Color lit = scenery_shade(needle, 26 + tone, 255);
        const SDL_Color dim = scenery_shade(needle, -14 + tone, 255);
        const SDL_Color tip = scenery_shade(needle, 18 + tone, 255);

        ragged_fan(renderer, tier_cx, tip_y, base_y + 7.0f, half_width * 1.02f, hang, under, under, under);
        ragged_fan(renderer, tier_cx, tip_y, base_y, half_width, hang, tip, lit, dim);
        paint_needles(renderer, tier_cx, tip_y, base_y, half_width, hang, lit, dim);

    }

}

static void paint_detail_tile(SDL_Renderer *renderer) {

    const float size = (float)DETAIL_TILE_SIZE;

    for (int i = 0; i < 12000; i++) {

        const _Bool is_dark = (scenery_noise_next() & 3u) != 0;
        const SDL_Color speck = is_dark ?
            (SDL_Color){104, 82, 54, (Uint8)scenery_noise_int(40, 110)} :
            (SDL_Color){240, 224, 188, (Uint8)scenery_noise_int(30, 90)};
        const int speck_size = scenery_noise_int(1, 3);
        const SDL_Rect rect = {scenery_noise_int(0, DETAIL_TILE_SIZE - 1), scenery_noise_int(0, DETAIL_TILE_SIZE - 1), speck_size, speck_size};
        SDL_SetRenderDrawColor(renderer, speck.r, speck.g, speck.b, speck.a);
        SDL_RenderFillRect(renderer, &rect);

    }

    for (int i = 0; i < 420; i++) {

        const float cx = scenery_noise_range(12.0f, size - 12.0f);
        const float cy = scenery_noise_range(12.0f, size - 12.0f);
        const float rx = scenery_noise_range(1.5f, 5.0f);
        const float ry = rx * scenery_noise_range(0.6f, 0.9f);
        const SDL_Color stone = scenery_shade((SDL_Color){158, 146, 128, 255}, scenery_noise_int(-24, 18), 255);
        scenery_fill_ellipse(renderer, cx + 1.0f, cy + 1.5f, rx, ry, (SDL_Color){90, 72, 50, 90});
        scenery_fill_ellipse(renderer, cx, cy, rx, ry, stone);
        scenery_fill_ellipse(renderer, cx - rx * 0.3f, cy - ry * 0.35f, rx * 0.45f, ry * 0.4f, scenery_shade(stone, 30, 200));

    }

}

static void to_ground_texels(float world_x, float world_y, float *x, float *y) {

    *x = (world_x - (float)ground_world.x) / GROUND_WORLD_PER_TEXEL;
    *y = (world_y - (float)ground_world.y) / GROUND_WORLD_PER_TEXEL;

}

static void fill_ground_ellipse(SDL_Renderer *renderer, float world_x, float world_y, float world_rx, float world_ry, SDL_Color color) {

    float x = 0.0f;
    float y = 0.0f;
    to_ground_texels(world_x, world_y, &x, &y);
    scenery_fill_ellipse(renderer, x, y, world_rx / GROUND_WORLD_PER_TEXEL, world_ry / GROUND_WORLD_PER_TEXEL, color);

}

static void fill_ground_soft_circle(SDL_Renderer *renderer, float world_x, float world_y, float world_radius, SDL_Color color) {

    float cx = 0.0f;
    float cy = 0.0f;
    to_ground_texels(world_x, world_y, &cx, &cy);
    const float outer = world_radius / GROUND_WORLD_PER_TEXEL;
    const float inner = maxf(outer - GROUND_FEATHER_TEXELS, 0.0f);
    const SDL_Color faded = {color.r, color.g, color.b, 0};

    SDL_Vertex vertices[GROUND_SEGMENTS * 2 + 1];
    int indices[GROUND_SEGMENTS * 9];
    vertices[0] = (SDL_Vertex){{cx, cy}, color, {0.0f, 0.0f}};
    for (int i = 0; i < GROUND_SEGMENTS; i++) {

        const float angle = (float)i * 6.2831853f / (float)GROUND_SEGMENTS;
        const float ca = cosf(angle);
        const float sa = sinf(angle);
        const int next = (i + 1) % GROUND_SEGMENTS;
        const int inner_a = 1 + i * 2;
        const int outer_a = 2 + i * 2;
        const int inner_b = 1 + next * 2;
        const int outer_b = 2 + next * 2;

        vertices[inner_a] = (SDL_Vertex){{cx + ca * inner, cy + sa * inner}, color, {0.0f, 0.0f}};
        vertices[outer_a] = (SDL_Vertex){{cx + ca * outer, cy + sa * outer}, faded, {0.0f, 0.0f}};

        const int triangle[9] = {0, inner_a, inner_b, inner_a, outer_a, outer_b, inner_a, outer_b, inner_b};
        for (int j = 0; j < 9; j++) indices[i * 9 + j] = triangle[j];

    }
    SDL_RenderGeometry(renderer, NULL, vertices, GROUND_SEGMENTS * 2 + 1, indices, GROUND_SEGMENTS * 9);

}

static void paint_ground_dirt(SDL_Renderer *renderer, const ForestBounds *bounds) {

    const SDL_Color base = {204, 180, 138, 255};
    const SDL_Color top = scenery_shade(base, 6, 255);
    const SDL_Color bottom = scenery_shade(base, -8, 255);

    float left = 0.0f;
    float upper = 0.0f;
    float right = 0.0f;
    float lower = 0.0f;
    to_ground_texels(bounds->left, bounds->top, &left, &upper);
    to_ground_texels(bounds->right, bounds->bottom, &right, &lower);

    const SDL_Vertex vertices[4] = {
        {{left, upper}, top, {0.0f, 0.0f}},
        {{right, upper}, top, {0.0f, 0.0f}},
        {{right, lower}, bottom, {0.0f, 0.0f}},
        {{left, lower}, bottom, {0.0f, 0.0f}}
    };
    const int indices[6] = {0, 1, 2, 0, 2, 3};
    SDL_RenderGeometry(renderer, NULL, vertices, 4, indices, 6);

    for (int i = 0; i < 240; i++) {

        const float cx = scenery_noise_range(bounds->left, bounds->right);
        const float cy = scenery_noise_range(bounds->top, bounds->bottom);
        const _Bool is_dark = (scenery_noise_next() & 1u) != 0;
        const SDL_Color tone = is_dark ? (SDL_Color){172, 146, 106, 5} : (SDL_Color){236, 218, 180, 8};
        const int blobs = scenery_noise_int(6, 10);
        for (int j = 0; j < blobs; j++) {
            fill_ground_ellipse(renderer, cx + scenery_noise_range(-260.0f, 260.0f), cy + scenery_noise_range(-150.0f, 150.0f),
                                scenery_noise_range(90.0f, 280.0f), scenery_noise_range(60.0f, 170.0f), tone);
        }

    }

}

static void paint_ground_meadow(SDL_Renderer *renderer, const ForestBounds *bounds) {

    const SDL_Color layers[5] = {{120, 140, 78, 255}, {104, 128, 66, 255}, {88, 114, 56, 255}, {72, 96, 48, 255}, {58, 78, 42, 255}};
    const float reach[5] = {190.0f, 140.0f, 90.0f, 30.0f, -50.0f};
    const float width = bounds->right - bounds->left;
    const float height = bounds->bottom - bounds->top;

    for (int layer = 0; layer < 5; layer++) {

        for (int edge = 0; edge < 4; edge++) {

            const float length = (edge == 0 || edge == 2) ? width : height;
            for (float along = -120.0f; along < length + 120.0f; along += scenery_noise_range(24.0f, 46.0f)) {

                const float radius = scenery_noise_range(40.0f, 72.0f);
                const float inward = reach[layer] - radius + scenery_noise_range(-22.0f, 22.0f);

                float x = bounds->left + along;
                float y = bounds->top + inward;
                if (edge == 1) { x = bounds->right - inward; y = bounds->top + along; }
                if (edge == 2) { x = bounds->left + along; y = bounds->bottom - inward; }
                if (edge == 3) { x = bounds->left + inward; y = bounds->top + along; }

                fill_ground_soft_circle(renderer, x, y, radius, scenery_shade(layers[layer], scenery_noise_int(-5, 5), 255));

            }

        }

    }

}

static float signed_distance_to_map(const ForestBounds *bounds, float x, float y) {

    const float dx = maxf(bounds->left - x, x - bounds->right);
    const float dy = maxf(bounds->top - y, y - bounds->bottom);
    if (dx > 0.0f || dy > 0.0f) {
        const float ox = maxf(dx, 0.0f);
        const float oy = maxf(dy, 0.0f);
        return sqrtf(ox * ox + oy * oy);
    }
    return maxf(dx, dy);

}

static float intrusion_into_map(const ForestBounds *bounds, const SDL_FRect *rect) {

    const float from_left = rect->x + rect->w - bounds->left;
    const float from_right = bounds->right - rect->x;
    const float from_top = rect->y + rect->h - bounds->top;
    const float from_bottom = bounds->bottom - rect->y;
    const float intrusion = minf(minf(from_left, from_right), minf(from_top, from_bottom));
    return maxf(intrusion, 0.0f);

}

static void add_instance(_Bool is_foreground, SDL_Texture *texture, const SDL_FRect *rect, Uint8 tint, _Bool casts_shadow) {

    const SceneryInstance instance = {
        texture,
        {(int)rect->x, (int)rect->y, (int)rect->w, (int)rect->h},
        tint,
        casts_shadow
    };

    if (is_foreground) {
        if (foreground_count < MAX_FOREGROUND_INSTANCES) foreground[foreground_count++] = instance;
    } else {
        if (background_count < MAX_BACKGROUND_INSTANCES) background[background_count++] = instance;
    }

}

static Uint8 depth_tint(float depth) {

    return (Uint8)(255.0f - minf(maxf(depth, 0.0f), NEAR_FOREST_DEPTH) * 0.065f);

}

static void bake_far_tree(SDL_Renderer *renderer, float x, float y) {

    SDL_Texture *texture = pines[scenery_noise_int(0, PINE_VARIANTS - 1)];
    const float scale = scenery_noise_range(PINE_MIN_SCALE, PINE_MAX_SCALE);
    const float w = PINE_WORLD_W * scale;
    const float h = PINE_WORLD_H * scale;
    const Uint8 tint = depth_tint(NEAR_FOREST_DEPTH);

    float tx = 0.0f;
    float ty = 0.0f;
    to_ground_texels(x - w * 0.5f, y - h, &tx, &ty);
    const SDL_FRect destination = {tx, ty, w / GROUND_WORLD_PER_TEXEL, h / GROUND_WORLD_PER_TEXEL};

    SDL_SetTextureColorMod(texture, tint, tint, tint);
    SDL_RenderCopyF(renderer, texture, NULL, &destination);
    SDL_SetTextureColorMod(texture, 255, 255, 255);

}

static _Bool plant_near_tree(const ForestBounds *bounds, float x, float y, float depth) {

    const float density = minf(0.3f + maxf(depth, 0.0f) / 420.0f, 0.95f);
    if (scenery_noise_range(0.0f, 1.0f) > density) return false;

    const float scale = scenery_noise_range(PINE_MIN_SCALE, PINE_MAX_SCALE);
    const SDL_FRect rect = {x - PINE_WORLD_W * scale * 0.5f, y - PINE_WORLD_H * scale, PINE_WORLD_W * scale, PINE_WORLD_H * scale};
    const float intrusion = intrusion_into_map(bounds, &rect);
    if (intrusion > BACKGROUND_TREE_INTRUSION) return false;

    const _Bool is_side = (x < bounds->left) || (x > bounds->right);
    const _Bool is_bottom = !is_side && (y > bounds->bottom);
    const float roll = scenery_noise_range(0.0f, 1.0f);
    const _Bool wants_foreground = (is_side && depth < FOREGROUND_SIDE_DEPTH && roll < 0.5f) ||
                                   (is_bottom && depth < FOREGROUND_BOTTOM_DEPTH && roll < 0.35f);
    const _Bool is_foreground = wants_foreground && intrusion <= FOREGROUND_INTRUSION;

    add_instance(is_foreground, pines[scenery_noise_int(0, PINE_VARIANTS - 1)], &rect, depth_tint(depth), true);
    return true;

}

static void plant_undergrowth(const ForestBounds *bounds, float x, float y, float depth) {

    if (depth < -UNDERGROWTH_INSIDE_REACH || depth > UNDERGROWTH_OUTSIDE_REACH) return;

    const _Bool is_side = (x < bounds->left) || (x > bounds->right);
    const float roll = scenery_noise_range(0.0f, 1.0f);

    if (roll < 0.42f) {

        const float scale = scenery_noise_range(0.8f, 1.3f);
        const float w = 64.0f * scale;
        const float h = 48.0f * scale;
        const SDL_FRect rect = {x - w * 0.5f, y - h, w, h};
        const _Bool is_foreground = is_side && scenery_noise_range(0.0f, 1.0f) < 0.3f && intrusion_into_map(bounds, &rect) <= FOREGROUND_INTRUSION;
        add_instance(is_foreground, bushes[scenery_noise_int(0, BUSH_VARIANTS - 1)], &rect, depth_tint(depth), false);

    } else if (roll < 0.9f) {

        const float scale = scenery_noise_range(0.8f, 1.2f);
        const float w = 96.0f * scale;
        const float h = 32.0f * scale;
        const SDL_FRect rect = {x - w * 0.5f, y - h, w, h};
        add_instance(false, tufts[scenery_noise_int(0, TUFT_VARIANTS - 1)], &rect, depth_tint(depth), false);

    }

}

static void plant_forest(SDL_Renderer *renderer, const ForestBounds *bounds) {

    const float left = (float)ground_world.x;
    const float right = (float)(ground_world.x + ground_world.w);
    const float top = (float)ground_world.y;
    const float bottom = (float)(ground_world.y + ground_world.h) + PINE_WORLD_H;

    for (float y = top; y < bottom; y += scenery_noise_range(46.0f, 70.0f)) {

        for (float x = left + scenery_noise_range(0.0f, 90.0f); x < right; x += scenery_noise_range(70.0f, 130.0f)) {

            const float depth = signed_distance_to_map(bounds, x, y);
            if (depth < -UNDERGROWTH_INSIDE_REACH - BACKGROUND_TREE_INTRUSION) continue;

            if (depth > NEAR_FOREST_DEPTH) {
                if (scenery_noise_range(0.0f, 1.0f) < 0.9f) bake_far_tree(renderer, x, y);
                continue;
            }

            if (!plant_near_tree(bounds, x, y, depth)) plant_undergrowth(bounds, x, y, depth);

        }

    }

}

static void bake_scenery(SDL_Renderer *renderer, const Map *map) {

    const ForestBounds bounds = {(float)map->mapLeftLimit, (float)map->mapRightLimit, (float)map->mapTopLimit, (float)map->mapBottomLimit};

    map_world = (SDL_Rect){map->mapLeftLimit, map->mapTopLimit, map->mapSizeX, map->mapSizeY};
    ground_world = (SDL_Rect){
        (int)(bounds.left - VIEW_MARGIN_X),
        (int)(bounds.top - VIEW_MARGIN_Y),
        (int)(bounds.right - bounds.left + VIEW_MARGIN_X * 2.0f),
        (int)(bounds.bottom - bounds.top + VIEW_MARGIN_Y * 2.0f)
    };
    background_count = 0;
    foreground_count = 0;

    SDL_Texture *previous = SDL_GetRenderTarget(renderer);
    SDL_BlendMode previous_blend = SDL_BLENDMODE_NONE;
    SDL_GetRenderDrawBlendMode(renderer, &previous_blend);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    scenery_noise_seed(FOREST_SEED);

    const SDL_Color needle_tones[4] = {{40, 70, 42, 255}, {46, 76, 40, 255}, {36, 64, 46, 255}, {52, 80, 44, 255}};
    for (int i = 0; i < PINE_VARIANTS; i++) {
        pines[i] = create_layer(renderer, PINE_TEXTURE_W, PINE_TEXTURE_H, (SDL_Color){34, 54, 32, 0});
        if (pines[i] != NULL) paint_pine(renderer, needle_tones[i % 4]);
    }

    for (int i = 0; i < BUSH_VARIANTS; i++) {
        bushes[i] = create_layer(renderer, BUSH_TEXTURE_W, BUSH_TEXTURE_H, (SDL_Color){50, 70, 36, 0});
        if (bushes[i] != NULL) scenery_bush(renderer, (float)BUSH_TEXTURE_W * 0.5f, (float)BUSH_TEXTURE_H * 0.58f, 32.0f);
    }

    for (int i = 0; i < TUFT_VARIANTS; i++) {
        tufts[i] = create_layer(renderer, TUFT_TEXTURE_W, TUFT_TEXTURE_H, (SDL_Color){80, 100, 50, 0});
        if (tufts[i] == NULL) continue;
        const int clumps = scenery_noise_int(6, 9);
        for (int j = 0; j < clumps; j++) {
            scenery_grass_tuft(renderer, scenery_noise_range(14.0f, (float)TUFT_TEXTURE_W - 14.0f), scenery_noise_range(44.0f, (float)TUFT_TEXTURE_H - 2.0f),
                               scenery_noise_range(14.0f, 26.0f), (SDL_Color){104, 128, 62, 255});
        }
    }

    shadow = create_layer(renderer, SHADOW_TEXTURE_W, SHADOW_TEXTURE_H, (SDL_Color){30, 26, 14, 0});
    if (shadow != NULL) {
        for (int i = 0; i < 24; i++) {
            const float k = 1.0f - (float)i * 0.04f;
            scenery_fill_ellipse(renderer, (float)SHADOW_TEXTURE_W * 0.5f, (float)SHADOW_TEXTURE_H * 0.5f,
                                 (float)SHADOW_TEXTURE_W * 0.5f * k, (float)SHADOW_TEXTURE_H * 0.5f * k, (SDL_Color){30, 26, 14, 6});
        }
    }

    detail_tile = create_layer(renderer, DETAIL_TILE_SIZE, DETAIL_TILE_SIZE, (SDL_Color){110, 90, 60, 0});
    if (detail_tile != NULL) paint_detail_tile(renderer);

    ground = create_layer(renderer, (int)((float)ground_world.w / GROUND_WORLD_PER_TEXEL), (int)((float)ground_world.h / GROUND_WORLD_PER_TEXEL), (SDL_Color){50, 66, 38, 255});
    if (ground != NULL) {
        paint_ground_dirt(renderer, &bounds);
        paint_ground_meadow(renderer, &bounds);
        plant_forest(renderer, &bounds);
    }

    SDL_SetRenderTarget(renderer, previous);
    SDL_SetRenderDrawBlendMode(renderer, previous_blend);

}

static _Bool world_rect_to_visible_screen(SDL_Rect *rect) {

    camera_world_to_screen(rect);
    return rect->x < (int)WINDOW_SIZE_X && rect->y < (int)WINDOW_SIZE_Y && rect->x + rect->w > 0 && rect->y + rect->h > 0;

}

static void render_detail_tile(SDL_Renderer *renderer) {

    if (detail_tile == NULL) return;

    const float half_view_w = WINDOW_SIZE_X * 0.5f / engine.camera->zoom;
    const float half_view_h = WINDOW_SIZE_Y * 0.5f / engine.camera->zoom;
    const int first_column = (int)maxf(0.0f, (engine.camera->position_x - half_view_w - (float)map_world.x) / (float)DETAIL_TILE_SIZE);
    const int first_row = (int)maxf(0.0f, (engine.camera->position_y - half_view_h - (float)map_world.y) / (float)DETAIL_TILE_SIZE);
    const int view_right = (int)(engine.camera->position_x + half_view_w) + 1;
    const int view_bottom = (int)(engine.camera->position_y + half_view_h) + 1;
    const int map_right = map_world.x + map_world.w;
    const int map_bottom = map_world.y + map_world.h;

    for (int row = first_row; ; row++) {

        const int y = map_world.y + row * DETAIL_TILE_SIZE;
        if (y >= map_bottom || y >= view_bottom) break;

        for (int column = first_column; ; column++) {

            const int x = map_world.x + column * DETAIL_TILE_SIZE;
            if (x >= map_right || x >= view_right) break;

            const int w = (x + DETAIL_TILE_SIZE > map_right) ? map_right - x : DETAIL_TILE_SIZE;
            const int h = (y + DETAIL_TILE_SIZE > map_bottom) ? map_bottom - y : DETAIL_TILE_SIZE;
            const SDL_Rect source = {0, 0, w, h};
            SDL_Rect destination = {x, y, w, h};
            if (world_rect_to_visible_screen(&destination)) SDL_RenderCopy(renderer, detail_tile, &source, &destination);

        }

    }

}

static void render_shadow(SDL_Renderer *renderer, const SceneryInstance *instance) {

    if (!instance->casts_shadow || shadow == NULL) return;

    const int w = instance->rect.w;
    const int h = instance->rect.w / 3;
    SDL_Rect destination = {
        instance->rect.x + w / 8,
        instance->rect.y + instance->rect.h - h / 2,
        w,
        h
    };
    if (world_rect_to_visible_screen(&destination)) SDL_RenderCopy(renderer, shadow, NULL, &destination);

}

static void render_instance(SDL_Renderer *renderer, const SceneryInstance *instance) {

    if (instance->texture == NULL) return;

    SDL_Rect destination = instance->rect;
    if (!world_rect_to_visible_screen(&destination)) return;

    SDL_SetTextureColorMod(instance->texture, instance->tint, instance->tint, instance->tint);
    SDL_RenderCopy(renderer, instance->texture, NULL, &destination);

}

void battlefield_scenery_render_background(SDL_Renderer *renderer, const Map *map) {

    if (ground == NULL) bake_scenery(renderer, map);
    if (ground == NULL) return;

    SDL_Rect destination = ground_world;
    camera_world_to_screen(&destination);
    SDL_RenderCopy(renderer, ground, NULL, &destination);

    render_detail_tile(renderer);

    for (unsigned int i = 0; i < foreground_count; i++) {
        render_shadow(renderer, &foreground[i]);
    }

    for (unsigned int i = 0; i < background_count; i++) {
        render_shadow(renderer, &background[i]);
        render_instance(renderer, &background[i]);
    }

}

void battlefield_scenery_render_foreground(SDL_Renderer *renderer) {

    for (unsigned int i = 0; i < foreground_count; i++) {
        render_instance(renderer, &foreground[i]);
    }

}

static void destroy_texture(SDL_Texture **texture) {

    if (*texture != NULL) SDL_DestroyTexture(*texture);
    *texture = NULL;

}

void battlefield_scenery_destroy(void) {

    destroy_texture(&ground);
    destroy_texture(&detail_tile);
    destroy_texture(&shadow);
    for (int i = 0; i < PINE_VARIANTS; i++) destroy_texture(&pines[i]);
    for (int i = 0; i < BUSH_VARIANTS; i++) destroy_texture(&bushes[i]);
    for (int i = 0; i < TUFT_VARIANTS; i++) destroy_texture(&tufts[i]);
    background_count = 0;
    foreground_count = 0;

}
