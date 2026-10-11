#ifndef AI_ORDER_INTERNAL_H
#define AI_ORDER_INTERNAL_H

#include "ai_order_types.h"
#include "battalion_types.h"

typedef struct BattalionOrderTable {

    enum OrderType *const type;
    enum BattalionMovementPace *const pace;
    unsigned int *const target;
    float *const dest_x;
    float *const dest_y;

} BattalionOrderTable;

#endif