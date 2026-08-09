#ifndef INVENTORY_INTERNAL_H
#define INVENTORY_INTERNAL_H

#include "inventory_types.h"
#include "general_internal.h"

typedef struct Inventory {

    unsigned short general_count;
    unsigned short item_count;
    unsigned short weapon_count;
    General *general;


} Inventory;

#endif