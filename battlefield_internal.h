#ifndef BATTLEFIELD_INTERNAL_H
#define BATTLEFIELD_INTERNAL_H

#include "battlefield_types.h"
#include "battleplan_battlefield_handoff.h"

typedef struct Battlefield {

    GeneralPayload *general_payload[2]; // 2 == max number of armies per battlefield

} Battlefield;

#endif