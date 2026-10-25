#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sched.h>

#include "shared_memory_control.h"
#include "memory_arena.h"

SharedMemoryControl *shared_memory_control_base_ptr = NULL;
BattalionTableSpan *battalion_table_span_base_ptr = NULL;
OrderBuffer *order_buffer_base_ptr = NULL;

static inline uint64_t signal_load(const SignalLine *s) {

    return __atomic_load_n(&s->value, __ATOMIC_ACQUIRE);

}

static inline void signal_store(SignalLine *s, uint64_t value) {

    __atomic_store_n(&s->value, value, __ATOMIC_RELEASE);

}

static inline void cpu_relax(void) {

    __builtin_ia32_pause();

}

SharedMemoryControl *shared_memory_control_create(void) {

    SharedMemoryControl *control = memory_arena_push(sizeof *control, SHARED_MEMORY_CACHE_LINE);

    if ((char *)control != (char *)memory_arena_base()) {

        fprintf(stderr, "shared memory control must be the first allocation\n");
        exit(EXIT_FAILURE);

    }

    memset(control, 0, sizeof *control);

    control->header.magic = SHARED_MEMORY_MAGIC;
    control->header.version = SHARED_MEMORY_VERSION;
    control->header.control_size = sizeof *control;

    return control;

}

BattalionTableSpan *shared_memory_span_create(SharedMemoryControl *control) {

    BattalionTableSpan *spans = memory_arena_push(sizeof *spans, SHARED_MEMORY_CACHE_LINE);

    memset(spans, 0, sizeof *spans);

    control->header.span_offset = (uint64_t)((char *)spans - (char *)memory_arena_base());
    control->header.span_size = sizeof *spans;

    return spans;

}

OrderBuffer *shared_memory_order_create(SharedMemoryControl *control) {

    OrderBuffer *orders = memory_arena_push(sizeof *orders, SHARED_MEMORY_CACHE_LINE);

    memset(orders, 0, sizeof *orders);

    control->header.order_offset = (uint64_t)((char *)orders - (char *)memory_arena_base());
    control->header.order_size = sizeof *orders;

    return orders;

}

void shared_memory_control_publish_layout(SharedMemoryControl *control) {

    signal_store(&control->layout_ready, 1);

}

uint64_t shared_memory_control_publish_step(SharedMemoryControl *control) {

    uint64_t next = signal_load(&control->c_step) + 1;

    signal_store(&control->c_step, next);

    return next;

}

enum SharedMemoryWaitResult shared_memory_control_wait_for_python(const SharedMemoryControl *control, uint64_t step, unsigned int max_spins) {

    for (unsigned int spins = 0; spins < max_spins; spins++) {

        if (signal_load(&control->py_step) == step) {
            return SHARED_MEMORY_WAIT_OK;
        }

        if (signal_load(&control->py_shutdown) != 0) {
            return SHARED_MEMORY_WAIT_SHUTDOWN;
        }

        if (spins < 1024) {
            cpu_relax();
        } else {
            sched_yield();
        }

    }

    return SHARED_MEMORY_WAIT_TIMEOUT;

}

void shared_memory_control_begin_battle(SharedMemoryControl *control, uint64_t battalion_count) {

    if (battalion_count > MAX_BATTALIONS) {
        fprintf(stderr, "battalion count %llu exceeds MAX_BATTALIONS\n", (unsigned long long)battalion_count);
        exit(EXIT_FAILURE);
    }

    signal_store(&control->battalion_count, battalion_count);
    signal_store(&control->episode_result, EPISODE_RUNNING);
    signal_store(&control->episode_index, signal_load(&control->episode_index) + 1);
    signal_store(&control->battle_active, 1);

}

void shared_memory_control_set_episode_result(SharedMemoryControl *control, enum EpisodeResult result) {

    signal_store(&control->episode_result, (uint64_t)result);

}

enum EpisodeResult shared_memory_control_episode_result(const SharedMemoryControl *control) {

    return (enum EpisodeResult)signal_load(&control->episode_result);

}

void shared_memory_control_end_battle(SharedMemoryControl *control) {

    signal_store(&control->battle_active, 0);

}

_Bool shared_memory_control_is_battle_active(const SharedMemoryControl *control) {

    return signal_load(&control->battle_active) != 0;

}

void shared_memory_control_shutdown(SharedMemoryControl *control) {

    signal_store(&control->battle_active, 0);
    signal_store(&control->c_shutdown, 1);

}

_Bool shared_memory_control_python_shutdown_requested(const SharedMemoryControl *control) {

    return signal_load(&control->py_shutdown) != 0;

}