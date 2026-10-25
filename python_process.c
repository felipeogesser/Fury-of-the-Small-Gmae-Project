#include "python_process.h"
#include <errno.h>
#include <signal.h>
#include <spawn.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <string.h>

extern char **environ;

static pid_t python_pid = -1;
static _Bool python_exited = false;

static void sleep_ms(unsigned int ms) {

    struct timespec ts = { (time_t)(ms / 1000), (long)(ms % 1000) * 1000000L };
    while (nanosleep(&ts, &ts) == -1 && errno == EINTR) {
    }

}

void python_process_spawn(const PythonProcessConfig *config) {

    const char *shm_name = config->shared_name;
    if (shm_name[0] == '/') shm_name++;

    char *argv[] = {
        (char *)config->python_path,
        (char *)config->script_path,
        "--shm",
        (char *)shm_name,
        "--mode",
        config->is_training ? "train" : "play",
        "--checkpoint",
        (char *)config->checkpoint_path,
        NULL
    };

    signed int status = posix_spawnp(&python_pid, config->python_path, NULL, NULL, argv, environ);
    if (status != 0) {
        fprintf(stderr, "failed to start python: %s\n", strerror(status));
        exit(EXIT_FAILURE);
    }

    python_exited = false;

}

_Bool python_process_is_alive(void) {

    if (python_pid <= 0 || python_exited) return false;

    signed int status = 0;
    pid_t result = waitpid(python_pid, &status, WNOHANG);

    if (result == 0) return true;

    python_exited = true;

    if (result == python_pid) {
        if (WIFEXITED(status)) {
            fprintf(stderr, "python exited with code %d\n", WEXITSTATUS(status));
        } else if (WIFSIGNALED(status)) {
            fprintf(stderr, "python killed by signal %d\n", WTERMSIG(status));
        }
    }

    return false;

}

enum PythonReadyResult python_process_wait_ready(const SharedMemoryControl *control, unsigned int timeout_ms) {

    for (unsigned int waited = 0; waited < timeout_ms; waited += 10) {

        if (shared_memory_control_is_python_ready(control)) return PYTHON_READY;
        if (!python_process_is_alive()) return PYTHON_DIED;

        sleep_ms(10);

    }

    return PYTHON_READY_TIMEOUT;

}

void python_process_stop(SharedMemoryControl *control, unsigned int timeout_ms) {

    shared_memory_control_shutdown(control);

    for (unsigned int waited = 0; waited < timeout_ms; waited += 10) {

        if (!python_process_is_alive()) return;
        sleep_ms(10);

    }

    kill(python_pid, SIGTERM);

    for (unsigned int waited = 0; waited < 1000; waited += 10) {

        if (!python_process_is_alive()) return;
        sleep_ms(10);

    }

    kill(python_pid, SIGKILL);
    waitpid(python_pid, NULL, 0);
    python_exited = true;

}