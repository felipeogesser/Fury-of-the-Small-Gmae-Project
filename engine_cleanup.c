#include "engine_cleanup.h"
#include "memory_arena.h"
#include "python_process.h"
#include "shared_memory_control.h"
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdbool.h>

static volatile sig_atomic_t termination_signal_count = 0;
static _Bool cleanup_done = false;

void engine_cleanup_run(void) {

    if (cleanup_done) return;
    cleanup_done = true;

    python_process_stop(shared_memory_control_base_ptr, 5000);

    shared_memory_control_base_ptr = NULL;
    battalion_table_span_base_ptr = NULL;
    order_buffer_base_ptr = NULL;

    memory_arena_unlink_name();
    memory_arena_free();

}

_Bool engine_cleanup_shutdown_requested(void) {

    return termination_signal_count > 0;

}

static void handle_termination_signal(int sig) {

    termination_signal_count++;

    if (termination_signal_count >= 2) {

        pid_t pid = python_process_pid();
        if (pid > 0) kill(pid, SIGKILL);
        memory_arena_unlink_name();
        _exit(128 + sig);

    }

}

static void handle_fatal_signal(int sig) {

    pid_t pid = python_process_pid();
    if (pid > 0) kill(pid, SIGKILL);
    memory_arena_unlink_name();

    signal(sig, SIG_DFL);
    raise(sig);

}

static void install_handler(int sig, void (*handler)(int), int flags) {

    struct sigaction action;
    memset(&action, 0, sizeof action);
    action.sa_handler = handler;
    action.sa_flags = flags;
    sigemptyset(&action.sa_mask);
    sigaction(sig, &action, NULL);

}

void engine_cleanup_install(void) {

    atexit(engine_cleanup_run);

    install_handler(SIGINT, handle_termination_signal, SA_RESTART);
    install_handler(SIGTERM, handle_termination_signal, SA_RESTART);
    install_handler(SIGHUP, handle_termination_signal, SA_RESTART);

    install_handler(SIGSEGV, handle_fatal_signal, SA_RESETHAND);
    install_handler(SIGBUS, handle_fatal_signal, SA_RESETHAND);
    install_handler(SIGFPE, handle_fatal_signal, SA_RESETHAND);
    install_handler(SIGILL, handle_fatal_signal, SA_RESETHAND);
    install_handler(SIGABRT, handle_fatal_signal, SA_RESETHAND);

}