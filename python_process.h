#ifndef PYTHON_PROCESS_H
#define PYTHON_PROCESS_H

#include "shared_memory_control.h"
#include <sys/types.h>

typedef struct PythonProcessConfig {
    const char *python_path;
    const char *script_path;
    const char *checkpoint_path;
    const char *shared_name;
    _Bool is_training;
} PythonProcessConfig;

enum PythonReadyResult {
    PYTHON_READY = 0,
    PYTHON_DIED = 1,
    PYTHON_READY_TIMEOUT = 2
};

void python_process_spawn(const PythonProcessConfig *config);
_Bool python_process_is_alive(void);
enum PythonReadyResult python_process_wait_ready(const SharedMemoryControl *control, unsigned int timeout_ms);
pid_t python_process_pid(void);
void python_process_stop(SharedMemoryControl *control, unsigned int timeout_ms);

#endif