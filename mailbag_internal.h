#ifndef MAILBAG_INTERNAL_H
#define MAILBAG_INTERNAL_H

typedef struct Mailbag {

    Letter *letters;
    unsigned int new_letter_count;

} Mailbag;

#endif