#ifndef MAILROOM_INTERNAL_H
#define MAILROOM_INTERNAL_H

#include "mailroom_types.h"
#include "mailbox_internal.h"
#include "scenes.h"

typedef struct Mailroom {

    Mailbox mailbox[SCENE_COUNT];

} Mailroom;

#endif