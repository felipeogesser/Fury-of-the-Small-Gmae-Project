#include "shared_memory_control.h"
#include "maps_internal.h"
#include "memory_arena.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <linux/futex.h>
#include <sys/syscall.h>
#include <time.h>
#include <unistd.h>

#define SHORT_SPIN 64

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

static void signal_wait(const SignalLine *s, uint64_t seen, unsigned int timeout_ms) {

    struct timespec timeout = { (time_t)(timeout_ms / 1000), (long)(timeout_ms % 1000) * 1000000L };
    syscall(SYS_futex, (const uint32_t *)&s->value, FUTEX_WAIT, (uint32_t)seen, &timeout, NULL, 0);

}

static void signal_wake(SignalLine *s) {

    syscall(SYS_futex, (uint32_t *)&s->value, FUTEX_WAKE, INT_MAX, NULL, NULL, 0);

}

SharedMemoryControl *shared_memory_control_create(void) {

    SharedMemoryControl *control = memory_arena_push(sizeof *control, SHARED_MEMORY_CACHE_LINE);

    if ((char *)control != (char *)memory_arena_base()) {

        fprintf(stderr, "shared memory control must be the first allocation\n");
        exit(EXIT_FAILURE);

    }

    memset(control, 0, sizeof *control);

    control->header.magic = SHARED_MEMORY_MAGIC;
    control->header.c_pid = (uint64_t)getpid();
    control->header.control_size = sizeof *control;

    return control;

}

_Bool shared_memory_control_is_python_ready(const SharedMemoryControl *control) {

    return signal_load(&control->py_ready) != 0;

}

uint64_t shared_memory_control_python_pid(const SharedMemoryControl *control) {

    return signal_load(&control->py_pid);

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
    signal_wake(&control->c_step);

    return next;

}

enum SharedMemoryWaitResult shared_memory_control_wait_for_python(SharedMemoryControl *control, uint64_t step, unsigned int timeout_ms) {

    for (unsigned int spins = 0; spins < SHORT_SPIN; spins++) {

        if (signal_load(&control->py_step) == step) return SHARED_MEMORY_WAIT_OK;
        cpu_relax();

    }

    uint64_t seen = signal_load(&control->py_step);
    if (seen == step) return SHARED_MEMORY_WAIT_OK;
    if (signal_load(&control->py_shutdown) != 0) return SHARED_MEMORY_WAIT_SHUTDOWN;

    signal_wait(&control->py_step, seen, timeout_ms);

    if (signal_load(&control->py_step) == step) return SHARED_MEMORY_WAIT_OK;
    if (signal_load(&control->py_shutdown) != 0) return SHARED_MEMORY_WAIT_SHUTDOWN;

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
    signal_wake(&control->c_step);

}

_Bool shared_memory_control_python_shutdown_requested(const SharedMemoryControl *control) {

    return signal_load(&control->py_shutdown) != 0;

}

void shared_memory_control_set_map(SharedMemoryControl *control, const Map *map) {

    SharedMemoryMapInfo *info = &control->map_info;

    info->left = (float)map->mapLeftLimit;
    info->top = (float)map->mapTopLimit;
    info->right = (float)map->mapRightLimit;
    info->bottom = (float)map->mapBottomLimit;
    info->width = (float)map->mapSizeX;
    info->height = (float)map->mapSizeY;

}

void shared_memory_control_set_timing(SharedMemoryControl *control, float tick_dt, uint32_t ticks_per_decision, uint64_t max_ticks) {

    SharedMemoryTimingInfo *info = &control->timing_info;

    info->tick_dt = tick_dt;
    info->ticks_per_decision = ticks_per_decision;
    info->max_ticks = max_ticks;

}