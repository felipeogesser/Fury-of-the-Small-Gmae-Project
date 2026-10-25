#ifndef SHARED_MEMORY_CONTROL_H
#define SHARED_MEMORY_CONTROL_H

#include <stdint.h>
#include <stddef.h>

#include "battalion_table_span.h"
#include "shared_memory_orders.h"

#define SHARED_MEMORY_MAGIC 0x53484D41u
#define SHARED_MEMORY_VERSION 2u
#define SHARED_MEMORY_CACHE_LINE 64u
#define SHARED_MEMORY_CONTROL_SIZE 1024u

enum EpisodeResult {
    EPISODE_RUNNING = 0,
    EPISODE_PLAYER_WON = 1,
    EPISODE_ENEMY_WON = 2,
    EPISODE_DRAW = 3,
    EPISODE_TIMEOUT = 4
};

enum SharedMemoryWaitResult {
    SHARED_MEMORY_WAIT_OK = 0,
    SHARED_MEMORY_WAIT_SHUTDOWN = 1,
    SHARED_MEMORY_WAIT_TIMEOUT = 2
};

typedef struct SignalLine {
    uint64_t value;
    uint8_t _pad[56];
} SignalLine;
#define SHARED_MEMORY_VERSION 3u

typedef struct SharedMemoryHeader {
    uint32_t magic;
    uint32_t version;
    uint64_t control_size;
    uint64_t span_offset;
    uint64_t span_size;
    uint64_t order_offset;
    uint64_t order_size;
    uint64_t c_pid;
    uint8_t _pad[8];
} SharedMemoryHeader;

typedef struct SharedMemoryControl {
    SharedMemoryHeader header;
    SignalLine layout_ready;
    SignalLine c_step;
    SignalLine py_step;
    SignalLine episode_result;
    SignalLine episode_index;
    SignalLine battalion_count;
    SignalLine battle_active;
    SignalLine c_shutdown;
    SignalLine py_shutdown;
    SignalLine py_ready;
    SignalLine py_pid;
    uint8_t _reserved[SHARED_MEMORY_CONTROL_SIZE - 12 * SHARED_MEMORY_CACHE_LINE];
} SharedMemoryControl;

_Static_assert(offsetof(SharedMemoryHeader, c_pid) == 48, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, py_ready) == 640, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, py_pid) == 704, "offset changed");
_Static_assert(sizeof(SignalLine) == 64, "SignalLine size changed");
_Static_assert(sizeof(SharedMemoryHeader) == 64, "SharedMemoryHeader size changed");
_Static_assert(sizeof(SharedMemoryControl) == SHARED_MEMORY_CONTROL_SIZE, "SharedMemoryControl size changed");
_Static_assert(offsetof(SharedMemoryControl, layout_ready) == 64, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, c_step) == 128, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, py_step) == 192, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, episode_result) == 256, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, episode_index) == 320, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, battalion_count) == 384, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, battle_active) == 448, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, c_shutdown) == 512, "offset changed");
_Static_assert(offsetof(SharedMemoryControl, py_shutdown) == 576, "offset changed");

extern SharedMemoryControl *shared_memory_control_base_ptr;
extern BattalionTableSpan *battalion_table_span_base_ptr;
extern OrderBuffer *order_buffer_base_ptr;

_Bool shared_memory_control_is_python_ready(const SharedMemoryControl *control);
uint64_t shared_memory_control_python_pid(const SharedMemoryControl *control);

SharedMemoryControl *shared_memory_control_create(void);
BattalionTableSpan *shared_memory_span_create(SharedMemoryControl *control);
OrderBuffer *shared_memory_order_create(SharedMemoryControl *control);

void shared_memory_control_publish_layout(SharedMemoryControl *control);
uint64_t shared_memory_control_publish_step(SharedMemoryControl *control);
enum SharedMemoryWaitResult shared_memory_control_wait_for_python(const SharedMemoryControl *control, uint64_t step, unsigned int max_spins);

void shared_memory_control_begin_battle(SharedMemoryControl *control, uint64_t battalion_count);
void shared_memory_control_set_episode_result(SharedMemoryControl *control, enum EpisodeResult result);
enum EpisodeResult shared_memory_control_episode_result(const SharedMemoryControl *control);
void shared_memory_control_end_battle(SharedMemoryControl *control);
_Bool shared_memory_control_is_battle_active(const SharedMemoryControl *control);

void shared_memory_control_shutdown(SharedMemoryControl *control);
_Bool shared_memory_control_python_shutdown_requested(const SharedMemoryControl *control);

#endif