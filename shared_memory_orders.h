#ifndef SHARED_MEMORY_ORDERS_H
#define SHARED_MEMORY_ORDERS_H

#include "table_types.h"
#include "maps_types.h"
#include <stdint.h>

#define MAX_BATTALIONS 20

enum OrderBufferSide {
    ORDER_BUFFER_PLAYER = 0,
    ORDER_BUFFER_ENEMY = 1,
    ORDER_BUFFER_SIDE_COUNT = 2
};

typedef struct OrderBufferArmy {
    int32_t type[MAX_BATTALIONS];
    int32_t pace[MAX_BATTALIONS];
    uint32_t target[MAX_BATTALIONS];
    float dest_x[MAX_BATTALIONS];
    float dest_y[MAX_BATTALIONS];
} OrderBufferArmy;

typedef struct OrderBuffer {
    OrderBufferArmy army[ORDER_BUFFER_SIDE_COUNT];
} OrderBuffer;

_Static_assert(sizeof(OrderBufferArmy) == 5 * 4 * MAX_BATTALIONS, "OrderBufferArmy size changed");
_Static_assert(sizeof(OrderBuffer) == 2 * 5 * 4 * MAX_BATTALIONS, "OrderBuffer size changed");

void order_buffer_apply(const OrderBufferArmy *orders, Table *tables, unsigned int battalion_count, const Map *map);

#endif