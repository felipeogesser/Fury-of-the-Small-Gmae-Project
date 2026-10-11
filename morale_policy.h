#ifndef MORALE_POLICY_H
#define MORALE_POLICY_H

#include "morale_types.h"

enum MoraleState evaluate_morale_state(unsigned int base, signed int current);
signed int evaluate_morale_static_modifiers(unsigned int modifiers);
signed int evaluate_morale_dynamic_modifiers(unsigned int modifiers, unsigned int max_hp, unsigned int current_hp, signed int balance_of_power);

#endif