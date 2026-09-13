#ifndef LETTER_INTERNAL_H
#define LETTER_INTERNAL_H

#include "letter_types.h"
#include "field_entry.h"
#include "scenes.h"

typedef struct Letter {

    enum Scene from;
    enum Scene to;
    void *body;
    FieldEntry *field_entry;

} Letter;

#endif