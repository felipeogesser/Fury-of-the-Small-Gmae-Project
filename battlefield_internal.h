#ifndef BATTLEFIELD_INTERNAL_H
#define BATTLEFIELD_INTERNAL_H

#include "battlefield_types.h"
#include "battleplan_battlefield_handoff.h"
#include "table_internal.h"

typedef struct Battlefield {

    GeneralPayload *general_payload[2]; // 2 == max number of armies per battlefield
    Table player_tables;
    Table enemy_tables;
    unsigned int battalion_count;
    unsigned int battalion_size;
    signed int balance_of_power; // range -10..10

} Battlefield;

#endif