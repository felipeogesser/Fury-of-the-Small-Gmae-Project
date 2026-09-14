#ifndef CONTRACT_H
#define CONTRACT_H

#include "contract_types.h"

Contract *contract_set(enum Contracts title, void *body);
_Bool check_contract(Contract *contract);
void *sign_contract(Contract *contract);
void contract_destroy(Contract *contract);


#endif