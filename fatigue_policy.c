#include "fatigue_policy.h"
#include "fatigue_types.h"

float evaluate_fatigue_current(unsigned int base, float current, _Bool is_active, _Bool is_fighting, float dt) {

    current += dt - dt * is_active * (2 + is_fighting);

    // keeps "current" between a range of 0-base
    current *= !(current < 0.0f);
    current = current * !(current > base) + base * (current > base);

    return current;

}

enum FatigueState evaluate_fatigue_state(unsigned int base, float current) {

    unsigned int bucket_size = base / FATIGUE_STATE_COUNT;
    signed int result = (base - current + (bucket_size)) / bucket_size - 1;
    enum FatigueState state = result * !(result < 0);
    state = (state < FATIGUE_STATE_COUNT) ? state : (FATIGUE_STATE_COUNT - 1);
    return state;

}
