#include "formation.h"
#include "direction_types.h"
#include "table_internal.h"
#include <math.h>

#include <stdio.h>

#define PI 3.14159274f
#define DIR_STEPS 4.0f   /* direction values for a full circle (0..4, 4 wraps to 0) */
#define DAMPING            0.85f  /* velocity kept per tick; lower = settles faster     */
#define STOP_SPEED         0.02f  /* slower than this per tick: velocity becomes 0      */
#define ARRIVE_GAIN        0.15f  /* desired speed per unit of distance to a slot       */
#define ARRIVE_WEIGHT      0.5f   /* how hard units steer toward slots / targets        */
#define SEPARATION_WEIGHT  0.6f   /* push strength between units that are too close     */
#define TURN_RATE 0.02f
#define FACING_MIN_STEP_FRACTION 0.3f
#define SLOT_DEADBAND 3.0f
#define FIGHTING_HOLD_RADIUS 48.0f

// private prototypes
static float wrap_direction(float dir);
static void direction_to_forward(float direction, float *forward_x, float *forward_y);
static float direction_from_vec(float vec_x, float vec_y, float fallback);
static float centered_offset(unsigned int index, unsigned int count, float step);
static unsigned int ceil_div(unsigned int a, unsigned int b);
static unsigned int min_uint(unsigned int a, unsigned int b);
static void formation_slot(unsigned int iter, unsigned int to_place, unsigned int files,
                    float center_x, float center_y,
                    float forward_x, float forward_y,
                    float step_depth, float step_side,
                    float *slot_x, float *slot_y);
static unsigned int count_in_formation(const _Bool *is_unit_present,
                                unsigned int b, unsigned int battalion_size,
                                unsigned int unit_count);
static void steer_arrive(float x, float y,
                  float target_x, float target_y, float max_step,
                  float *vec_x, float *vec_y);
static void steer_separation(const float *unit_x, const float *unit_y,
                      const _Bool *is_unit_present,
                      unsigned int self, unsigned int battalion_size,
                      unsigned int unit_count,
                      float radius, float *force_x, float *force_y);
/*static void integrate_unit(float *x, float *y, float *vx, float *vy,
                    float force_x, float force_y, float max_speed);*/
static void clamp_length(float *x, float *y, float max);
static float turn_toward(float current, float desired, float max_turn);

void formation_update(Table *tables,
                const unsigned int battalion_count, const unsigned int unit_count,
                const unsigned int battalion_size,
                const float unit_w, const float unit_h, const float padding_x, const float padding_y,
                const float distance, const float catch_up) {

    const float *unit_x = tables->unit_tables.friendly_dynamic.x;
    const float *unit_y = tables->unit_tables.friendly_dynamic.y;
    float *unit_vx = tables->unit_tables.friendly_dynamic.vec_x;
    float *unit_vy = tables->unit_tables.friendly_dynamic.vec_y;
    //enum Direction *general_direction = tables->general_tables.friendly_dynamic.direction;
    const enum BattalionMovementMode *movement_state = tables->battalion_tables.friendly_dynamic.movement_state;

    const _Bool *is_unit_present = tables->unit_tables.friendly_dynamic.is_present;
    const _Bool *is_headless = tables->battalion_tables.friendly_dynamic.is_headless;
    const float *speed = tables->battalion_tables.friendly_dynamic.speed;
    const unsigned char *formation_x = tables->battalion_tables.friendly_static.formation_x;
    const unsigned char *formation_y = tables->battalion_tables.friendly_static.formation_y;
    const float *general_x = tables->general_tables.friendly_dynamic.x;
    const float *general_y = tables->general_tables.friendly_dynamic.y;
    const float *general_vec_x = tables->general_tables.friendly_dynamic.vec_x;
    const float *general_vec_y = tables->general_tables.friendly_dynamic.vec_y;
    float *general_facing = tables->general_tables.friendly_dynamic.facing;

    float step_depth = unit_h + padding_x;   /* between ranks */
    float step_side  = unit_w + padding_y;   /* between units in a rank */

    /* separation only below the smaller unit size, so units resting in
       their slots feel no push */
    float sep_radius = unit_w < unit_h ? unit_w : unit_h;

    for (unsigned int b = 0; b < battalion_count; b++) {
        if (is_headless[b]) continue;   /* steer_headless_units handles those */

        unsigned int ranks = formation_x[b];
        unsigned int files = formation_y[b];
        if (ranks == 0 || files == 0) {
            printf("err\n");
            continue;
        }
        //float direction = direction_from_vec(general_vec_x[b], general_vec_y[b],
        //                             general_direction[b] == LEFT ? 2.0f : 0.0f);
        float general_step = sqrtf(general_vec_x[b] * general_vec_x[b] + general_vec_y[b] * general_vec_y[b]);
        if (general_step > speed[b] * FACING_MIN_STEP_FRACTION) {
            float desired = direction_from_vec(general_vec_x[b], general_vec_y[b], general_facing[b]);
            general_facing[b] = turn_toward(general_facing[b], desired, TURN_RATE);
        }
        float direction = general_facing[b];
        unsigned int to_place = min_uint(count_in_formation(is_unit_present,
                                                            b, battalion_size, unit_count),
                                                            ranks * files);
        if (to_place == 0) continue;

        float forward_x = 0.0f, forward_y = 0.0f;
        direction_to_forward(direction, &forward_x, &forward_y);

        /* front rank sits `distance` behind the general; the block's center is
        half its depth further back */
        unsigned int ranks_used = ceil_div(to_place, files);
        float half_depth = ((float)ranks_used - 1.0f) / 2.0f * step_depth;

        float center_x = general_x[b] - (distance + half_depth) * forward_x;
        float center_y = general_y[b] - (distance + half_depth) * forward_y;

        /* the most a unit of this battalion may move in one tick */
        float max_step = speed[b] * catch_up;

        unsigned int iter = 0;   /* slot number within this battalion */

        for (unsigned int i = b * battalion_size, k = 0; k < battalion_size && iter < to_place; k++) {
            unsigned int idx = i + k;
            if (idx >= unit_count) break;
            if (!is_unit_present[idx]) continue;

            /* the unit keeps its slot number even while it is away fighting,
               so the other units don't reshuffle */
            unsigned int slot = iter;
            iter++;

            /* goes for its target instead: get_close_to_engaged_target moves it */
            if (is_unit_going_for_target(tables, idx, battalion_size)) continue;

            float slot_x = 0.0f, slot_y = 0.0f;
            formation_slot(slot, to_place, files, center_x, center_y,
                           forward_x, forward_y, step_depth, step_side,
                           &slot_x, &slot_y);

            if (movement_state[b] == FIGHTING) {
                float hold_dx = slot_x - unit_x[idx];
                float hold_dy = slot_y - unit_y[idx];
                if (hold_dx * hold_dx + hold_dy * hold_dy < FIGHTING_HOLD_RADIUS * FIGHTING_HOLD_RADIUS) {
                    steer_separation(unit_x, unit_y, is_unit_present, idx, battalion_size, unit_count,
                                     sep_radius, &unit_vx[idx], &unit_vy[idx]);
                    continue;
                }
            }

            steer_arrive(unit_x[idx], unit_y[idx], slot_x, slot_y, max_step,
                         &unit_vx[idx], &unit_vy[idx]);
            steer_separation(unit_x, unit_y, is_unit_present, idx, battalion_size, unit_count,
                             sep_radius, &unit_vx[idx], &unit_vy[idx]);
        }
    }
}

_Bool is_unit_going_for_target(const Table *tables, unsigned int idx, unsigned int battalion_size) {
    unsigned int b = idx / battalion_size;
    if (!tables->unit_tables.friendly_dynamic.is_engaged[idx]) return 0;

    enum BattalionMovementMode movement_state = tables->battalion_tables.friendly_dynamic.movement_state[b];
    _Bool is_headless = tables->battalion_tables.friendly_dynamic.is_headless[b];
    _Bool is_attacking = tables->battalion_order.type[b] == ORDER_ATTACK;
    _Bool is_running = (movement_state == RUNNING) | (movement_state == CHARGING) | (movement_state == FIGHTING);
    if (!is_headless && !(is_attacking && is_running)) return 0;

    unsigned int target_idx = tables->unit_tables.friendly_dynamic.target_idx[idx];
    enum TargetType target_type = tables->unit_tables.friendly_dynamic.target_type[idx];
    if (target_type == TARGET_UNIT) return tables->unit_tables.enemy_dynamic.is_present[target_idx] & (tables->unit_tables.enemy_dynamic.hp[target_idx] > 0.0f);
    if (target_type == TARGET_GENERAL) return tables->general_tables.enemy_dynamic.is_present[target_idx] & (tables->general_tables.enemy_dynamic.hp[target_idx] > 0.0f);
    return 0;
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
static float direction_from_vec(float vec_x, float vec_y, float fallback) {
    if (vec_x * vec_x + vec_y * vec_y < 0.0001f) return fallback;

    float angle = atan2f(vec_y, vec_x);                              /* -PI..PI */
    return wrap_direction(angle * (DIR_STEPS / (2.0f * PI)));       /* 0..4 */
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

/* How many units of battalion b take a formation slot (every present unit;
   engaged ones keep theirs). Needed before placing, because centering the
   ranks depends on it. */
static unsigned int count_in_formation(const _Bool *is_unit_present,
                                unsigned int b, unsigned int battalion_size,
                                unsigned int unit_count) {
    unsigned int n = 0;
    for (unsigned int i = b * battalion_size, k = 0; k < battalion_size; k++) {
        unsigned int idx = i + k;
        if (idx >= unit_count) break;
        if (is_unit_present[idx]) n++;
    }
    return n;
}

/* Step toward a point, slowing down as it gets close. Used for slots.
   Adds the step straight into vec (vec is zero at the start of the tick). */
static void steer_arrive(float x, float y,
                  float target_x, float target_y, float max_step,
                  float *vec_x, float *vec_y) {
    float dx = target_x - x;
    float dy = target_y - y;
    if (dx * dx + dy * dy < SLOT_DEADBAND * SLOT_DEADBAND) return;
    float want_x = dx * ARRIVE_GAIN;
    float want_y = dy * ARRIVE_GAIN;
    clamp_length(&want_x, &want_y, max_step);
    *vec_x += want_x;
    *vec_y += want_y;
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
/*
//Apply force to velocity, damp it, cap it, stop it if tiny, then move.
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
}*/

/* Shorten (x, y) to at most `max`. */
static void clamp_length(float *x, float *y, float max) {
    float len2 = *x * *x + *y * *y;
    if (len2 > max * max) {
        float k = max / sqrtf(len2);
        *x *= k;
        *y *= k;
    }
}

static float turn_toward(float current, float desired, float max_turn) {
    float diff = wrap_direction(desired - current);
    if (diff > DIR_STEPS / 2.0f) diff -= DIR_STEPS;
    if (diff > max_turn) diff = max_turn;
    if (diff < -max_turn) diff = -max_turn;
    return wrap_direction(current + diff);
}