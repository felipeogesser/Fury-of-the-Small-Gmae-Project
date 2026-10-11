#ifndef AI_ORDER_TYPES_H
#define AI_ORDER_TYPES_H

typedef struct BattalionOrderTable BattalionOrderTable;

enum OrderType {

    UNKNOWN_ORDER = -1,
    ORDER_HOLD,
    ORDER_MOVE,
    ORDER_ATTACK

};

extern const ComplexFieldEntry *const battalion_order_table_field_table_ptr;

#endif