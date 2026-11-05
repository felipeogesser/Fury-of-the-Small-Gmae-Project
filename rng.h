#ifndef RNG_H
#define RNG_H

#include <stdint.h>

enum RngStream {
    RNG_BATTLE,
    RNG_COSMETIC,
    RNG_STREAM_COUNT
};

void rng_init(void);
uint64_t rng_run_seed(void);
void rng_begin_battle(uint64_t battle_index);
uint32_t rng_next(enum RngStream stream);
uint32_t rng_below(enum RngStream stream, uint32_t bound);
float rng_unit(enum RngStream stream);

#endif
