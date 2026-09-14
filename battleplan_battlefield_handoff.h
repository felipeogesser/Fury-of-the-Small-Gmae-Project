#ifndef BATTLEPLAN_BATTLEFIELD_H
#define BATTLEPLAN_BATTLEFIELD_H

#include "general_types.h"

typedef struct position {

    unsigned int x, y;

} Position;

typedef struct GeneralAndPosition {

    General *general;
    Position pos;

} GeneralAndPosition;

typedef struct GeneralPayload {

    unsigned int general_count;
    GeneralAndPosition *general_and_pos;

} GeneralPayload;

#endif