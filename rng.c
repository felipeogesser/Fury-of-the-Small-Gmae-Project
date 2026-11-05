#include "rng.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define RNG_SEED_VARIABLE "CGAME_SEED"

typedef struct Rng {

    uint64_t state;
    uint64_t increment;

} Rng;

static uint64_t run_seed = 0;
static Rng streams[RNG_STREAM_COUNT];

static uint64_t splitmix64(uint64_t x) {

    uint64_t z = x + 0x9E3779B97F4A7C15ull;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ull;
    z = (z ^ (z >> 27)) * 0x94D049BB133111EBull;
    return z ^ (z >> 31);

}

static uint32_t pcg32(Rng *rng) {

    uint64_t old = rng->state;
    rng->state = old * 6364136223846793005ull + rng->increment;
    uint32_t xorshifted = (uint32_t)(((old >> 18) ^ old) >> 27);
    uint32_t rotation = (uint32_t)(old >> 59);
    return (xorshifted >> rotation) | (xorshifted << ((32u - rotation) & 31u));

}

static void rng_seed(Rng *rng, uint64_t seed, uint64_t sequence) {

    rng->state = 0;
    rng->increment = (sequence << 1) | 1u;
    pcg32(rng);
    rng->state += seed;
    pcg32(rng);

}

static uint64_t seed_from_environment(void) {

    const char *value = getenv(RNG_SEED_VARIABLE);
    if (value != NULL && value[0] != '\0') {
        char *end = NULL;
        unsigned long long parsed = strtoull(value, &end, 10);
        if (end != NULL && *end == '\0') return (uint64_t)parsed;
        fprintf(stderr, "%s='%s' is not a number, using a random seed\n", RNG_SEED_VARIABLE, value);
    }

    struct timespec now;
    clock_gettime(CLOCK_REALTIME, &now);
    uint64_t mixed = splitmix64((uint64_t)now.tv_sec ^ ((uint64_t)now.tv_nsec << 20) ^ ((uint64_t)getpid() << 40));
    return mixed >> 1;

}

void rng_init(void) {

    run_seed = seed_from_environment();

    char text[32];
    snprintf(text, sizeof text, "%" PRIu64, run_seed);
    setenv(RNG_SEED_VARIABLE, text, 1);
    printf("seed %s\n", text);

    rng_seed(&streams[RNG_COSMETIC], splitmix64(run_seed), RNG_COSMETIC);
    rng_begin_battle(0);

}

uint64_t rng_run_seed(void) {

    return run_seed;

}

void rng_begin_battle(uint64_t battle_index) {

    uint64_t seed = splitmix64(run_seed ^ splitmix64(battle_index));
    rng_seed(&streams[RNG_BATTLE], seed, RNG_BATTLE);

}

uint32_t rng_next(enum RngStream stream) {

    return pcg32(&streams[stream]);

}

uint32_t rng_below(enum RngStream stream, uint32_t bound) {

    return (uint32_t)(((uint64_t)pcg32(&streams[stream]) * bound) >> 32);

}

float rng_unit(enum RngStream stream) {

    return (float)(pcg32(&streams[stream]) >> 8) * (1.0f / 16777216.0f);

}
