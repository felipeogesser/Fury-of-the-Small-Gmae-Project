#ifndef AI_ORDER_INTERNAL_H
#define AI_ORDER_INTERNAL_H

#include "ai_order_types.h"
#include "battalion_types.h"

typedef struct BattalionOrderTable {

    enum OrderType *type;
    enum BattalionMovementPace *pace;
    unsigned int *target;
    float *dest_x;
    float *dest_y;

} BattalionOrderTable;

#endif