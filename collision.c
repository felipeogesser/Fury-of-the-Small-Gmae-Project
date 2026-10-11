#include "collision.h"
#include <math.h>

#define COLLISION_SOFTNESS 0.8f   /* fraction of an overlap fixed per pass, 0..1        */

// private prototypes
static void separate_pair(float *ax, float *ay, float *bx, float *by, float min_dist);

/* ---------------------------------------------------------------------- */
/* Collisions (run after all movement, for every unit, headless or not)   */
/* ---------------------------------------------------------------------- */

/* Resolve overlaps inside one army. 2 to 4 passes is usually enough.
   Stops each scan early once all alive units have been seen. */
void resolve_collisions(float *unit_x, float *unit_y, const _Bool *unit_alive,
                        unsigned int unit_count, unsigned int unit_alive_count,
                        float min_dist, unsigned int passes) {
    for (unsigned int p = 0; p < passes; p++) {
        unsigned int remaining_a = unit_alive_count;
        for (unsigned int a = 0; a < unit_count && remaining_a > 1; a++) {
            if (!unit_alive[a]) continue;
            remaining_a--;                    /* alive units still after a */

            unsigned int remaining_b = remaining_a;
            for (unsigned int b = a + 1; b < unit_count && remaining_b > 0; b++) {
                if (!unit_alive[b]) continue;
                remaining_b--;
                separate_pair(&unit_x[a], &unit_y[a], &unit_x[b], &unit_y[b], min_dist);
            }
        }
    }
}

/* Resolve overlaps between two armies. */
void resolve_collisions_between(float *a_x, float *a_y, const _Bool *a_alive,
                                unsigned int a_count, unsigned int a_alive_count,
                                float *b_x, float *b_y, const _Bool *b_alive,
                                unsigned int b_count, unsigned int b_alive_count,
                                float min_dist, unsigned int passes) {
    for (unsigned int p = 0; p < passes; p++) {
        unsigned int remaining_a = a_alive_count;
        for (unsigned int a = 0; a < a_count && remaining_a > 0; a++) {
            if (!a_alive[a]) continue;
            remaining_a--;

            unsigned int remaining_b = b_alive_count;
            for (unsigned int b = 0; b < b_count && remaining_b > 0; b++) {
                if (!b_alive[b]) continue;
                remaining_b--;
                separate_pair(&a_x[a], &a_y[a], &b_x[b], &b_y[b], min_dist);
            }
        }
    }
}

/* Push two units apart so they are at least min_dist apart, half each. */
static void separate_pair(float *ax, float *ay, float *bx, float *by, float min_dist) {
    float dx = *bx - *ax;
    float dy = *by - *ay;
    float d2 = dx * dx + dy * dy;
    if (d2 >= min_dist * min_dist) return;

    if (d2 < 0.000001f) {                     /* exactly stacked: split on x */
        float half = min_dist * 0.5f * COLLISION_SOFTNESS;
        *ax -= half;
        *bx += half;
        return;
    }
    float d = sqrtf(d2);
    float push = (min_dist - d) * 0.5f * COLLISION_SOFTNESS / d;
    *ax -= dx * push;  *ay -= dy * push;
    *bx += dx * push;  *by += dy * push;
}
/* ---------------------------------------------------------------------- */
/* Order per tick                                                         */
/* ---------------------------------------------------------------------- */
/*
    1. Your decision code fills headless_mode, anchor, direction and target
       for each headless battalion.

    2. Movement, both over the whole army:
         place_army(...)            battalions with a general
         steer_headless_army(...)   headless battalions

    3. Collisions, after all movement:
         resolve_collisions(friendly..., friendly_count, friendly_alive_count, min_dist, 3);
         resolve_collisions(enemy...,    enemy_count,    enemy_alive_count,    min_dist, 3);
         resolve_collisions_between(friendly..., enemy..., min_dist, 3);

    min_dist is usually the smaller of unit_w and unit_h.
*/