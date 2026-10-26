#ifndef BATTLEPLAN_BATTLEFIELD_H
#define BATTLEPLAN_BATTLEFIELD_H

#include "general_internal.h"
#include "position.h"

typedef struct GeneralAndPosition {

    General general;
    Position pos;

} GeneralAndPosition;

typedef struct GeneralPayload {

    unsigned int general_count;
    GeneralAndPosition *general_and_pos;

} GeneralPayload;

#endif