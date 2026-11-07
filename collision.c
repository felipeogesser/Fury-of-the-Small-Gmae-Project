#include "collision.h"
#include <math.h>

#define COLLISION_SOFTNESS 0.8f

// private prototypes
static void separate_pair(float *ax, float *ay, float *bx, float *by, float min_dist);
static _Bool battalion_box(const float *x, const float *y, const _Bool *alive, unsigned int first, unsigned int count, float *box);

void resolve_collisions(float *unit_x, float *unit_y, const _Bool *unit_alive,
                        unsigned int unit_count, float min_dist, unsigned int passes) {
    for (unsigned int p = 0; p < passes; p++) {
        for (unsigned int a = 0; a < unit_count; a++) {
            if (!unit_alive[a]) continue;

            for (unsigned int b = a + 1; b < unit_count; b++) {
                if (!unit_alive[b]) continue;
                separate_pair(&unit_x[a], &unit_y[a], &unit_x[b], &unit_y[b], min_dist);
            }
        }
    }
}

void resolve_collisions_by_battalion(float *unit_x, float *unit_y, const _Bool *unit_alive,
                                     unsigned int battalion_count, unsigned int battalion_size,
                                     float min_dist, unsigned int passes) {

    const float reach = min_dist * 2.0f;
    float box[battalion_count][4];
    _Bool has_units[battalion_count];
    const unsigned int unit_count = battalion_count * battalion_size;
    for (unsigned int p = 0; p < passes; p++) {
        for (unsigned int bb = 0; bb < battalion_count; bb++) {
            has_units[bb] = battalion_box(unit_x, unit_y, unit_alive, bb * battalion_size, battalion_size, box[bb]);
        }
        for (unsigned int a = 0; a < unit_count; a++) {
            if (!unit_alive[a]) continue;

            for (unsigned int bb = a / battalion_size; bb < battalion_count; bb++) {
                if (!has_units[bb]) continue;
                if (unit_x[a] < box[bb][0] - reach || unit_x[a] > box[bb][1] + reach ||
                    unit_y[a] < box[bb][2] - reach || unit_y[a] > box[bb][3] + reach) continue;

                const unsigned int start = (bb == a / battalion_size) ? a + 1 : bb * battalion_size;
                const unsigned int end = (bb + 1) * battalion_size;
                for (unsigned int b = start; b < end; b++) {
                    if (!unit_alive[b]) continue;
                    separate_pair(&unit_x[a], &unit_y[a], &unit_x[b], &unit_y[b], min_dist);
                }
            }
        }
    }
}

static _Bool battalion_box(const float *x, const float *y, const _Bool *alive, unsigned int first, unsigned int count, float *box) {
    _Bool any = 0;
    for (unsigned int k = first; k < first + count; k++) {
        if (!alive[k]) continue;
        if (!any) {
            box[0] = box[1] = x[k];
            box[2] = box[3] = y[k];
            any = 1;
            continue;
        }
        if (x[k] < box[0]) box[0] = x[k];
        if (x[k] > box[1]) box[1] = x[k];
        if (y[k] < box[2]) box[2] = y[k];
        if (y[k] > box[3]) box[3] = y[k];
    }
    return any;
}

void resolve_collisions_between(float *a_x, float *a_y, const _Bool *a_alive,
                                unsigned int a_count,
                                float *b_x, float *b_y, const _Bool *b_alive,
                                unsigned int b_count,
                                float min_dist, unsigned int passes) {
    for (unsigned int p = 0; p < passes; p++) {
        for (unsigned int a = 0; a < a_count; a++) {
            if (!a_alive[a]) continue;

            for (unsigned int b = 0; b < b_count; b++) {
                if (!b_alive[b]) continue;
                separate_pair(&a_x[a], &a_y[a], &b_x[b], &b_y[b], min_dist);
            }
        }
    }
}

static void separate_pair(float *ax, float *ay, float *bx, float *by, float min_dist) {
    float dx = *bx - *ax;
    float dy = *by - *ay;
    float d2 = dx * dx + dy * dy;
    if (d2 >= min_dist * min_dist) return;

    if (d2 < 0.000001f) {
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
