#include "memory_arena.h"
#include "engine_internal.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdbool.h>

struct Arena {
    char *base;
    char *current;
    size_t capacity;
};

enum MemoryArenaAllocStatus {
    MEMORY_ARENA_ALLOC_SUCCESS = 0,
    MEMORY_ARENA_ALLOC_FAILED
};

// private prototypes
static enum MemoryArenaAllocStatus memory_arena_alloc(void);

static enum MemoryArenaAllocStatus memory_arena_alloc_status;
static struct Arena arena = {0};
static char shared_name[64] = {0};
static _Bool shared_name_linked = false;

static enum MemoryArenaAllocStatus memory_arena_alloc(void) {

    size_t capacity = (size_t)(104857600ULL);
    arena.capacity = capacity;

    snprintf(shared_name, sizeof shared_name, "/battle_arena_%ld", (long)getpid());
    shm_unlink(shared_name);

    signed int fd = shm_open(shared_name, O_CREAT | O_EXCL | O_RDWR, 0600);
    if (fd == -1) {
        perror("shm_open");
        return MEMORY_ARENA_ALLOC_FAILED;
    }
    shared_name_linked = true;

    if (ftruncate(fd, (off_t)capacity) == -1) {
        perror("ftruncate");
        close(fd);
        memory_arena_unlink_name();
        return MEMORY_ARENA_ALLOC_FAILED;
    }

    void *shared = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (shared == MAP_FAILED) {
        perror("mmap");
        memory_arena_unlink_name();
        return MEMORY_ARENA_ALLOC_FAILED;
    }

    madvise(shared, capacity, MADV_HUGEPAGE);

    arena.base = shared;

    return MEMORY_ARENA_ALLOC_SUCCESS;

}

const char *memory_arena_shared_name(void) {

    return shared_name;

}

void memory_arena_unlink_name(void) {

    if (shared_name_linked) {
        shm_unlink(shared_name);
        shared_name_linked = false;
    }

}

void memory_arena_free(void) {

    if (arena.base == NULL) return;

    memset(arena.base, 0, arena.capacity);

    if (munmap(arena.base, arena.capacity) != 0) {
        perror("munmap");
        fprintf(stderr, "memory arena freeing failed\n");
    }

    arena.base = NULL;
    arena.current = NULL;

    memory_arena_unlink_name();

}

void memory_arena_init(void) {

    memory_arena_alloc_status = memory_arena_alloc();

    if (memory_arena_alloc_status == MEMORY_ARENA_ALLOC_FAILED) {
        
        fprintf(stderr, "memory arena initialization failed\n");
        exit(EXIT_FAILURE);

    }
    
    arena.current = arena.base;
    
}

void memory_arena_reset(void) {

    memset(arena.base, 0, arena.capacity);

    arena.current = arena.base;

}

void memory_arena_reset_to_address(void *p) {

    memset(p, 0, arena.current - (char *)p);
    arena.current = (char *)p;

}

char *memory_arena_base(void) {

    return arena.base;

}

char *memory_arena_current_pointer(void) {

    return arena.current;

}

size_t memory_arena_memory_used(void) {
    
    return (size_t)(arena.current - arena.base);

}

size_t memory_arena_memory_remainder(void) {

    return arena.capacity - (size_t)(arena.current - arena.base) + 64; // 64 bytes for simulating padding

}

size_t memory_arena_memory_capacity(void) {

    return arena.capacity;

}

void *memory_arena_push(size_t size, size_t alignment) {

    char *p = arena.current;

    p = (char *)(((uintptr_t)p + alignment - 1) & ~(alignment - 1));

    arena.current = p + size;

    if (arena.current > arena.base + arena.capacity) {
        
        fprintf(stderr, "memory allocated bigger than limit\n");
        exit(EXIT_FAILURE);
        
    }

    return p;

}