#include "memory_arena.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Arena {
    char *base;
    char *current;
    size_t capacity;
};

enum MemoryArenaAllocStatus {
    MEMORY_ARENA_ALLOC_SUCCESS = 0,
    MEMORY_ARENA_ALLOC_FAILED
};

static enum MemoryArenaAllocStatus memory_arena_alloc_status;

static struct Arena arena = {0};

static enum MemoryArenaAllocStatus memory_arena_alloc(void) {

    size_t capacity = 8388608;
    arena.capacity = capacity;
    
    void *ptr = mmap(
        NULL,
        capacity,
        PROT_READ | PROT_WRITE,
        MAP_PRIVATE | MAP_ANONYMOUS | MAP_HUGETLB,
        -1,
        0
    );

    if (ptr == MAP_FAILED) {

        ptr = mmap(
            NULL,
            capacity,
            PROT_READ | PROT_WRITE,
            MAP_PRIVATE | MAP_ANONYMOUS,
            -1,
            0
        );


        if (ptr != MAP_FAILED) {
            madvise(ptr, capacity, MADV_HUGEPAGE);
        }

    }

    if (ptr == MAP_FAILED) {
        return MEMORY_ARENA_ALLOC_FAILED;
    }
    
    arena.base = ptr;

    return MEMORY_ARENA_ALLOC_SUCCESS;

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

void memory_arena_free(void) {

    memset(arena.base, 0, arena.capacity);

    if (munmap(arena.base, arena.capacity) != 0) {
        
        perror("munmap");
        
        fprintf(stderr, "memory arena freeing failed\n");
        exit(EXIT_FAILURE);

    }

}

char *memory_arena_current_pointer(void) {

    return arena.current;

}

size_t memory_arena_memory_used(void) {
    
    return (size_t)(arena.current - arena.base);

}

size_t memory_arena_memory_remainder(void) {

    return arena.capacity - memory_arena_memory_used();

}

size_t memory_arena_memory_capacity(void) {

    return arena.capacity;

}
