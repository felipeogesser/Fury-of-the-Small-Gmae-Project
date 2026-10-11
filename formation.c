#include "formation.h"
#include "direction_types.h"
#include "table_internal.hs"
#include <math.h>

#include <stdio.h>

#define PI 3.14159274f
#define DIR_STEPS 4.0f   /* direction values for a full circle (0..4, 4 wraps to 0) */

// private prototypes
static float wrap_direction(float dir);
static void direction_to_forward(float direction, float *forward_x, float *forward_y);
static _Bool direction_from_movement(float move_x, float move_y, float *direction);
static float centered_offset(unsigned int index, unsigned int count, float step);
static unsigned int ceil_div(unsigned int a, unsigned int b);
static unsigned int min_uint(unsigned int a, unsigned int b);
static void formation_slot(unsigned int iter, unsigned int to_place, unsigned int files,
                    float center_x, float center_y,
                    float forward_x, float forward_y,
                    float step_depth, float step_side,
                    float *slot_x, float *slot_y);
static unsigned int count_in_formation(const _Bool *is_unit_present, const _Bool *is_unit_engaged,
                                unsigned int b, unsigned int battalion_size,
                                unsigned int unit_count);
static void steer_arrive(float x, float y, float vx, float vy,
                  float target_x, float target_y, float max_speed,
                  float *force_x, float *force_y);
static void steer_separation(const float *unit_x, const float *unit_y,
                      const _Bool *is_unit_present,
                      unsigned int self, unsigned int battalion_size,
                      unsigned int unit_count,
                      float radius, float *force_x, float *force_y);
static void integrate_unit(float *x, float *y, float *vx, float *vy,
                    float force_x, float force_y, float max_speed);

/* ---------------------------------------------------------------------- */
/* Main function                                                          */
/* ---------------------------------------------------------------------- */

/* Moves every unit of an army toward its slot in its battalion's formation.

   Battalion b follows general b. The formation is centered on the tip of the
   general's S line (`distance`), facing the way the general walks. Front rank
   fills first, dead and engaged units are skipped so the others close the
   gaps, the last rank is centered, and the block re-centers as it shrinks.

   Skipped battalions: headless, engaged, or general not moving.
   Skipped units: dead or engaged.

   unit_x, unit_y        unit positions, moved in place       [unit_count]
   unit_vx, unit_vy      unit velocities, updated in place    [unit_count]
   unit_alive            alive flag per unit                  [unit_count]
   unit_engaged          engaged flag per unit                [unit_count]
   unit_count            total units in the army
   battalion_size        units per battalion
   general_x, general_y  general positions                    [per battalion]
   general_move_x/_y     general movement this tick           [per battalion]
   headless              battalion has no general             [per battalion]
   battalion_engaged     battalion is in combat               [per battalion]
   formation_x           ranks (front to back)                [per battalion]
   formation_y           units per rank (side by side)        [per battalion]
   unit_w                side-to-side room per unit
   unit_h                front-to-back room per unit
   padding_x             gap between ranks
   padding_y             gap between units in a rank
   distance              S: how far ahead of the general the block's center sits
   max_speed             max distance a unit moves per tick */
void formation_update(Table *tables,
                unsigned int battalion_count, unsigned int unit_count,
                unsigned int battalion_size,
                float unit_w, float unit_h, float padding_x, float padding_y,
                float distance, float max_speed) {

    float *unit_x = tables->unit_tables.friendly_dynamic.x;
    float *unit_y = tables->unit_tables.friendly_dynamic.y;
    float *unit_vx = tables->unit_tables.friendly_dynamic.vec_x;
    float *unit_vy = tables->unit_tables.friendly_dynamic.vec_y;
    enum Direction *direction = tables->battalion_tables.friendly_dynamic.direction;

    const _Bool *is_unit_present = tables->unit_tables.friendly_dynamic.is_present;
    const _Bool *is_unit_engaged = tables->unit_tables.friendly_dynamic.is_engaged;
    const _Bool *is_battalion_engaged = tables->battalion_tables.friendly_dynamic.is_engaged;
    const _Bool *is_headless = tables->battalion_tables.friendly_dynamic.is_headless;
    const unsigned char *formation_x = tables->battalion_tables.friendly_dynamic.formation_x;
    const unsigned char *formation_y = tables->battalion_tables.friendly_dynamic.formation_y;
    const float *battalion_target_x = tables->battalion_tables.friendly_dynamic.battalion_target_x;
    const float *battalion_target_y = tables->battalion_tables.friendly_dynamic.battalion_target_y;
    const float *general_x = tables->general_tables.friendly_dynamic.x;
    const float *general_y = tables->general_tables.friednly_dynamic.y;
    const float *general_vec_x = tables->general_tables.friednly_dynamic.vec_x;
    const float *general_vec_y = tables->general_tables.friednly_dynamic.vec_y;

    float step_depth = unit_h + padding_x;   /* between ranks */
    float step_side  = unit_w + padding_y;   /* between units in a rank */

    /* separation only below the smaller unit size, so units resting in
       their slots feel no push */
    float sep_radius = unit_w < unit_h ? unit_w : unit_h;

    for (unsigned int b = 0; b < battalion_count; b++) {
        if (is_headless[b] || is_battalion_engaged[b]) continue;

        unsigned int ranks = formation_x[b];
        unsigned int files = formation_y[b];
        if (ranks == 0 || files == 0) {
            printf("err\n");
            continue;
        }
        if (!direction_from_movement(general_vec_x[b], general_vec_y[b], &direction[b]))
            continue;

        unsigned int to_place = min_uint(count_in_formation(is_unit_present, is_unit_engaged,
                                                            b, battalion_size, unit_count),
                                         ranks * files);
        if (to_place == 0) continue;

        float forward_x = 0.0f, forward_y = 0.0f;
        direction_to_forward(direction[b], &forward_x, &forward_y);

        /* center of the block: tip of the general's S line */
        float center_x = general_x[b] + distance * forward_x;
        float center_y = general_y[b] + distance * forward_y;

        unsigned int iter = 0;   /* slot number within this battalion */

        for (unsigned int idx = b * battalion_size, k = 0; k < battalion_size && iter < to_place; k++) {
            idx += k;
            if (idx >= unit_count) break;
            if (!is_unit_present[idx] || unit_engaged[idx]) continue;

            float slot_x = 0.0f, slot_y = 0.0f;
            formation_slot(iter, to_place, files, center_x, center_y,
                           forward_x, forward_y, step_depth, step_side,
                           &slot_x, &slot_y);

            float fx = 0.0f, fy = 0.0f;
            steer_arrive(unit_x[idx], unit_y[idx], unit_vx[idx], unit_vy[idx],
                         slot_x, slot_y, max_speed, &fx, &fy);
            steer_separation(unit_x, unit_y, is_unit_present, idx, battalion_size, unit_count,
                             sep_radius, &fx, &fy);
            integrate_unit(&unit_x[idx], &unit_y[idx], &unit_vx[idx], &unit_vy[idx],
                           fx, fy, max_speed);

            iter++;
        }
    }
}


/* ---------------------------------------------------------------------- */
/* Helpers                                                                */
/* ---------------------------------------------------------------------- */

/* Keep direction inside 0..DIR_STEPS, wrapping negatives and overflow. */
static float wrap_direction(float dir) {
    dir = fmodf(dir, DIR_STEPS);
    if (dir < 0.0f) dir += DIR_STEPS;
    return dir;
}

/* Unit vector pointing the way the battalion faces. */
static void direction_to_forward(float direction, float *forward_x, float *forward_y) {
    float angle = wrap_direction(direction) * (2.0f * PI / DIR_STEPS);
    *forward_x = cosf(angle);
    *forward_y = sinf(angle);
}

/* Converts a movement vector (how far the general walks on each axis this
   tick) into a direction (0..4). Returns 0 and leaves *direction untouched
   if the general isn't moving, since a zero vector has no angle. */
static _Bool direction_from_movement(float move_x, float move_y, float *direction) {
    if (move_x * move_x + move_y * move_y < 0.0001f) return 0;

    float angle = atan2f(move_y, move_x);                              /* -PI..PI */
    *direction = wrap_direction(angle * (DIR_STEPS / (2.0f * PI)));   /* 0..4 */
    return 1;
}

/* Offset of slot `index` in a centered line of `count` slots.
   count 5 -> -2, -1, 0, 1, 2 (times step)
   count 4 -> -1.5, -0.5, 0.5, 1.5 (times step) */
static float centered_offset(unsigned int index, unsigned int count, float step) {
    return ((float)index - ((float)count - 1.0f) / 2.0f) * step;
}

/* Integer division rounding up. */
static unsigned int ceil_div(unsigned int a, unsigned int b) {
    return (a + b - 1) / b;
}

static unsigned int min_uint(unsigned int a, unsigned int b) {
    return a < b ? a : b;
}

/* Position of slot `iter` in a centered formation of `to_place` units. */
static void formation_slot(unsigned int iter, unsigned int to_place, unsigned int files,
                    float center_x, float center_y,
                    float forward_x, float forward_y,
                    float step_depth, float step_side,
                    float *slot_x, float *slot_y) {
    unsigned int ranks_used = ceil_div(to_place, files);
    unsigned int rank = iter / files;
    unsigned int file = iter % files;
    unsigned int in_rank = min_uint(to_place - rank * files, files);

    float depth = -centered_offset(rank, ranks_used, step_depth);
    float t     =  centered_offset(file, in_rank, step_side);

    *slot_x = center_x + depth * forward_x - t * forward_y;
    *slot_y = center_y + depth * forward_y + t * forward_x;
}

/* How many units of battalion b take a formation slot (alive, not engaged).
   Needed before placing, because centering the ranks depends on it. */
static unsigned int count_in_formation(const _Bool *is_unit_present, const _Bool *is_unit_engaged,
                                unsigned int b, unsigned int battalion_size,
                                unsigned int unit_count) {
    unsigned int n = 0;
    for (unsigned int idx = b * battalion_size, k = 0; k < battalion_size; k++) {
        idx += k;
        if (idx >= unit_count) break;
        if (is_unit_present[idx] && !is_unit_engaged[idx]) n++;
    }
    return n;
}

/* Steer toward a point, slowing down as it gets close. Used for slots. */
static void steer_arrive(float x, float y, float vx, float vy,
                  float target_x, float target_y, float max_speed,
                  float *force_x, float *force_y) {
    float want_x = (target_x - x) * ARRIVE_GAIN;
    float want_y = (target_y - y) * ARRIVE_GAIN;
    clamp_length(&want_x, &want_y, max_speed);
    *force_x += (want_x - vx) * ARRIVE_WEIGHT;
    *force_y += (want_y - vy) * ARRIVE_WEIGHT;
}

/* Push unit `self` away from battalion mates closer than `radius`.
   Stronger the deeper the overlap, zero at the radius edge. */
static void steer_separation(const float *unit_x, const float *unit_y,
                      const _Bool *is_unit_present,
                      unsigned int self, unsigned int battalion_size,
                      unsigned int unit_count,
                      float radius, float *force_x, float *force_y) {
    unsigned int b = self / battalion_size;
    for (unsigned int k = 0; k < battalion_size; k++) {
        unsigned int j = b * battalion_size + k;
        if (j >= unit_count) break;
        if (j == self || !is_unit_present[j]) continue;
        float dx = unit_x[self] - unit_x[j];
        float dy = unit_y[self] - unit_y[j];
        float d2 = dx * dx + dy * dy;
        if (d2 >= radius * radius) continue;

        if (d2 < 0.000001f) {                 /* exactly stacked: pick a side */
            *force_x += (self < j ? -1.0f : 1.0f) * SEPARATION_WEIGHT;
            continue;
        }
        float d = sqrtf(d2);
        float strength = (radius - d) / radius * SEPARATION_WEIGHT;
        *force_x += dx / d * strength;
        *force_y += dy / d * strength;
    }
}

/* Apply force to velocity, damp it, cap it, stop it if tiny, then move. */
static void integrate_unit(float *x, float *y, float *vx, float *vy,
                    float force_x, float force_y, float max_speed) {
    *vx = (*vx + force_x) * DAMPING;
    *vy = (*vy + force_y) * DAMPING;
    clamp_length(vx, vy, max_speed);
    if (*vx * *vx + *vy * *vy < STOP_SPEED * STOP_SPEED) {
        *vx = 0.0f;
        *vy = 0.0f;
    }
    *x += *vx;
    *y += *vy;
}
