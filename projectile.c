#include "projectile.h"
#include "projectile_internal.h"
#include "rng.h"
#include "battalion_table_internal.h"
#include "camera.h"
#include "combat.h"
#include "direction_types.h"
#include "engine_internal.h"
#include "general_table_internal.h"
#include "sprites_internal.h"
#include "table_internal.h"
#include "target_types.h"
#include "unit_table_internal.h"
#include <math.h>
#include <stdlib.h>
#include <SDL2/SDL.h>

#define PI 3.14159274f
#define MAX_PROJECTILES 4096
#define ARROW_SPEED 700.0f         // ground distance covered per second
#define ARROW_MIN_FLIGHT 0.1f
#define ARROW_SCATTER 0.05f        // landing spread, as a fraction of the shot distance
#define ARROW_ARC 0.25f            // peak height of the drawn arc, as a fraction of the shot distance
#define ARROW_HIT_RADIUS 18.0f     // an arrow hits the closest enemy within this distance of where it lands
#define ARROW_SHIELD_BLOCK 50.0f   // percent of arrows a shield stops, only when the target faces the arrow
#define BATTALION_SPREAD 400.0f
#define SPRITE_CENTER 50.0f        // entities are drawn from the top-left of a 100x100 frame

// private prototypes
static void resolve_impact(const Projectile *projectile, Table *victims, unsigned int battalion_count, unsigned int battalion_size);
static float random_spread(void);

static Projectile projectiles[MAX_PROJECTILES];
static unsigned int projectile_count = 0;

void projectile_reset(void) {

    projectile_count = 0;

}

void projectile_launch(const ProjectileLaunch *launch, float dt) {

    if (projectile_count >= MAX_PROJECTILES) return;

    float dx = launch->target_x - launch->x;
    float dy = launch->target_y - launch->y;
    float distance = sqrtf(dx * dx + dy * dy);

    // aim where the target will be when the arrow comes down
    float flight_time = fmaxf(distance / ARROW_SPEED, ARROW_MIN_FLIGHT);
    float flight_ticks = (dt > 0.0f) ? flight_time / dt : 0.0f;
    float aim_x = launch->target_x + launch->target_vec_x * flight_ticks;
    float aim_y = launch->target_y + launch->target_vec_y * flight_ticks;

    float scatter = distance * ARROW_SCATTER;
    aim_x += scatter * random_spread();
    aim_y += scatter * random_spread();

    dx = aim_x - launch->x;
    dy = aim_y - launch->y;
    distance = sqrtf(dx * dx + dy * dy);

    Projectile *projectile = &projectiles[projectile_count++];
    projectile->side = launch->side;
    projectile->start_x = launch->x;
    projectile->start_y = launch->y;
    projectile->aim_x = aim_x;
    projectile->aim_y = aim_y;
    projectile->elapsed = 0.0f;
    projectile->flight_time = fmaxf(distance / ARROW_SPEED, ARROW_MIN_FLIGHT);
    projectile->damage = launch->damage;
    projectile->is_armor_piercing = launch->is_armor_piercing;

}

void projectile_update(Table *player_tables, Table *enemy_tables, unsigned int battalion_count, unsigned int battalion_size, float dt) {

    for (unsigned int i = 0; i < projectile_count;) {

        Projectile *projectile = &projectiles[i];
        projectile->elapsed += dt;
        if (projectile->elapsed < projectile->flight_time) {
            i++;
            continue;
        }

        Table *victims = (projectile->side == ARMY_SIDE_LEFT) ? enemy_tables : player_tables;
        resolve_impact(projectile, victims, battalion_count, battalion_size);

        // landed: the last arrow takes its slot, so i is checked again
        projectiles[i] = projectiles[--projectile_count];

    }

}

void projectile_render(void) {

    const Sprite *sprite = &engine.sprite_pack->projectile[PROJECTILE_ARROW];
    if (sprite->texture == NULL) return;

    SDL_Renderer *renderer = engine.renderer;
    const signed int width = sprite->width / sprite->frames_count;
    const signed int height = sprite->height;

    for (unsigned int i = 0; i < projectile_count; i++) {

        const Projectile *projectile = &projectiles[i];
        const float t = projectile->elapsed / projectile->flight_time;
        const float dx = projectile->aim_x - projectile->start_x;
        const float dy = projectile->aim_y - projectile->start_y;
        const float distance = sqrtf(dx * dx + dy * dy);

        // the arrow travels in a straight line on the ground, the arc is only lifted on screen
        const float peak = ARROW_ARC * distance;
        const float lift = 4.0f * peak * t * (1.0f - t);
        const float lift_slope = 4.0f * peak * (1.0f - 2.0f * t);
        const float x = projectile->start_x + dx * t;
        const float y = projectile->start_y + dy * t - lift;
        const float angle = atan2f(dy - lift_slope, dx) * (180.0f / PI);

        SDL_Rect position = {
            (signed int)(x + SPRITE_CENTER) - width / 2,
            (signed int)(y + SPRITE_CENTER) - height / 2,
            width,
            height
        };
        camera_world_to_screen(&position);

        SDL_RenderCopyEx(renderer, sprite->texture, NULL, &position, (double)angle, NULL, SDL_FLIP_NONE);

    }

}

static void resolve_impact(const Projectile *projectile, Table *victims, unsigned int battalion_count, unsigned int battalion_size) {

    const float x = projectile->aim_x;
    const float y = projectile->aim_y;
    const float reach = BATTALION_SPREAD + ARROW_HIT_RADIUS;
    float best_dist = ARROW_HIT_RADIUS * ARROW_HIT_RADIUS;
    enum TargetType best_type = UNKNOWN_TARGET;
    unsigned int best_idx = -1U;

    const BattalionDynamicTable *battalions = &victims->battalion_tables.friendly_dynamic;
    const UnitDynamicTable *units = &victims->unit_tables.friendly_dynamic;
    const GeneralDynamicTable *generals = &victims->general_tables.friendly_dynamic;

    for (unsigned int b = 0; b < battalion_count; b++) {

        if (!battalions->is_present[b]) continue;

        const float bdx = battalions->x[b] - x;
        const float bdy = battalions->y[b] - y;
        if (bdx * bdx + bdy * bdy > reach * reach) continue;

        for (unsigned int k = 0; k < battalion_size; k++) {

            const unsigned int idx = b * battalion_size + k;
            if (!units->is_present[idx] || units->hp[idx] <= 0.0f) continue;

            const float dx = units->x[idx] - x;
            const float dy = units->y[idx] - y;
            const float dist = dx * dx + dy * dy;
            if (dist < best_dist) {
                best_dist = dist;
                best_type = TARGET_UNIT;
                best_idx = idx;
            }

        }

    }

    // bodyguards: a general is only hit when no soldier stands within reach of the arrow,
    // otherwise volleys snipe generals and leave whole battalions headless
    if (best_type == UNKNOWN_TARGET) {

        for (unsigned int b = 0; b < battalion_count; b++) {

            if (!generals->is_present[b] || generals->hp[b] <= 0.0f) continue;

            const float dx = generals->x[b] - x;
            const float dy = generals->y[b] - y;
            const float dist = dx * dx + dy * dy;
            if (dist < best_dist) {
                best_dist = dist;
                best_type = TARGET_GENERAL;
                best_idx = b;
            }

        }

    }

    if (best_type == UNKNOWN_TARGET) return;

    float *hp = NULL;
    float armor = 0.0f;
    _Bool is_shielded = false;
    enum Direction facing = UNKNOWN_DIRECTION;
    if (best_type == TARGET_UNIT) {

        const unsigned int b = best_idx / battalion_size;
        hp = &victims->unit_tables.friendly_dynamic.hp[best_idx];
        armor = victims->battalion_tables.friendly_static.armor[b];
        is_shielded = victims->battalion_tables.friendly_static.is_shielded[b];
        facing = victims->unit_tables.friendly_dynamic.direction[best_idx];

    } else {

        hp = &victims->general_tables.friendly_dynamic.hp[best_idx];
        armor = victims->general_tables.friendly_static.armor[best_idx];
        is_shielded = victims->general_tables.friendly_static.is_shielded[best_idx];
        facing = victims->general_tables.friendly_dynamic.direction[best_idx];

    }

    // a shield only covers the side its bearer is facing
    const _Bool flies_right = projectile->aim_x > projectile->start_x;
    const _Bool faces_arrow = flies_right ? (facing == LEFT) : (facing == RIGHT);
    if (is_shielded && faces_arrow && roll_hit(ARROW_SHIELD_BLOCK)) return;

    armor = (projectile->is_armor_piercing) ? armor / 2 : armor;
    *hp -= apply_armor(projectile->damage, armor);

}

// roughly bell shaped in -1..1, so most arrows land near the aim point
static float random_spread(void) {

    const float a = rng_unit(RNG_BATTLE);
    const float b = rng_unit(RNG_BATTLE);
    return a + b - 1.0f;

}
