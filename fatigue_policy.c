#include "fatigue_policy.h"
#include "fatigue_types.h"
#include <SDL2/SDL.h>

float evaluate_fatigue_current(unsigned int base, float current, _Bool is_active, float dt) {

    current += dt - dt * 2 * is_active;

    // keeps "current" between a range of 0-base
    current *= !(current < 0.0f);
    current = current * !(current > base) + base * (current > base);

    return current;

}

enum FatigueState evaluate_fatigue_state(unsigned int base, float current) {

    unsigned int aux = base / FATIGUE_STATE_COUNT;
    current = (current < 1.0f) ? 1.0f : current;
    return (base - current + (aux)) / aux - 1;

}
