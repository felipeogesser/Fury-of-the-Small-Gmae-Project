#ifndef MORALE_POLICY_H
#define MORALE_POLICY_H

signed int evaluate_static_morale_modifiers(unsigned int modifiers);
signed int evaluate_dynamic_morale_modifiers(unsigned int modifiers, unsigned int max_hp, unsigned int current_hp, signed int balance_of_power);

#endif