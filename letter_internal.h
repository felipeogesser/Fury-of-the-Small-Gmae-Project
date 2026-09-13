#ifndef LETTER_INTERNAL_H
#define LETTER_INTERNAL_H

#include "letter_types.h"
#include "field_entry.h"

typedef struct Letter {

    void *body;
    FieldEntry *field_entry;

} Letter;

#endif