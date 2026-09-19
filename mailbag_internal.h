#ifndef MAILBAG_INTERNAL_H
#define MAILBAG_INTERNAL_H

#include "mailbag_types.h"
#include "letter_types.h"

typedef struct Mailbag {

    Letter *letters;
    unsigned int new_letter_count;

} Mailbag;

#endif