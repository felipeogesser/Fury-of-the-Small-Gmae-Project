#ifndef CONTRACT_INTERNAL_H
#define CONTRACT_INTERNAL_H

#include "contract_types.h"

typedef struct Contract {

    enum Contracts title;
    void *body;

} Contract;

#endif