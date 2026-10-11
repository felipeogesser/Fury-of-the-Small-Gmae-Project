#ifndef FATIGUE_POLICY_H
#define FATIGUE_POLICY_H

#include "fatigue_types.h"

float evaluate_fatigue_current(unsigned int base, float current, _Bool is_active, float dt);

enum FatigueState evaluate_fatigue_state(unsigned int base, unsigned int current);

#endif