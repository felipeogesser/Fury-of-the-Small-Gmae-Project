#include "shared_memory_orders.h"
#include "ai_order_internal.h"
#include "battalion_types.h"
#include "maps_internal.h"
#include "table_internal.h"
#include <math.h>
#include <stdint.h>

static _Bool order_type_is_valid(int32_t type) {

    return type == ORDER_HOLD || type == ORDER_MOVE || type == ORDER_ATTACK;

}

static _Bool order_pace_is_valid(int32_t pace) {

    return pace == PACE_WALK || pace == PACE_RUN;

}

static float clamp_float(float value, float min, float max) {

    return fminf(fmaxf(value, min), max);

}

void order_buffer_apply(const OrderBufferArmy *orders, Table *tables, unsigned int battalion_count, const Map *map) {

    BattalionOrderTable *order = &tables->battalion_order;
    unsigned int count = (battalion_count < MAX_BATTALIONS) ? battalion_count : MAX_BATTALIONS;
    float left = (float)map->mapLeftLimit;
    float right = (float)map->mapRightLimit;
    float top = (float)map->mapTopLimit;
    float bottom = (float)map->mapBottomLimit;

    for (unsigned int i = 0; i < count; i++) {

        int32_t type = orders->type[i];
        int32_t pace = orders->pace[i];
        uint32_t target = orders->target[i];
        float dest_x = orders->dest_x[i];
        float dest_y = orders->dest_y[i];
        _Bool dest_is_valid = isfinite(dest_x) && isfinite(dest_y);

        if (!order_type_is_valid(type)) type = ORDER_HOLD;
        if (!order_pace_is_valid(pace)) pace = PACE_WALK;
        if (type == ORDER_ATTACK && target >= battalion_count) type = ORDER_HOLD;
        if (type == ORDER_MOVE && !dest_is_valid) type = ORDER_HOLD;
        if (target >= battalion_count) target = 0;

        if (dest_is_valid) {
            dest_x = clamp_float(dest_x, left, right);
            dest_y = clamp_float(dest_y, top, bottom);
        } else {
            dest_x = order->dest_x[i];
            dest_y = order->dest_y[i];
        }

        order->type[i] = (enum OrderType)type;
        order->pace[i] = (enum BattalionMovementPace)pace;
        order->target[i] = target;
        order->dest_x[i] = dest_x;
        order->dest_y[i] = dest_y;

    }

}