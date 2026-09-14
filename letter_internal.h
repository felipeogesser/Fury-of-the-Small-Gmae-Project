#ifndef LETTER_INTERNAL_H
#define LETTER_INTERNAL_H

#include "letter_types.h"
#include "contract_types.h"
#include "scenes.h"

typedef struct Letter {

    Contract *contract;
    enum Scene from;
    enum Scene to;

} Letter;

#endif