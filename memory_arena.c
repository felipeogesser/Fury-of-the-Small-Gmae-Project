#include "memory_arena.h"
#include "engine_internal.h"
#include <sys/mman.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <fcntl.h>
#include <unistd.h>

#define SHARED_NAME "/battle_shared"

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

    size_t capacity = (size_t)(104857600ULL);
    arena.capacity = capacity;

    if (!engine.has_program_been_initialized_with_ai_bypass) {
        
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

    } else {

        signed int fd = shm_open(SHARED_NAME, O_CREAT | O_RDWR, 0600);
        if (fd == -1) {
            perror("shm_open");
            exit(EXIT_FAILURE);
        }
        if (ftruncate(fd, capacity) == -1) {
            perror("ftruncate");
            exit(EXIT_FAILURE);
        }

        void *shared = mmap(NULL, capacity, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
        if (shared == MAP_FAILED) {
            perror("mmap");
            return MEMORY_ARENA_ALLOC_FAILED
        }
        close(fd);

        arena.base = shared;
        return MEMORY_ARENA_ALLOC_SUCCESS;

    }

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

    memset(p, 0, arena.current);
    arena.current = (char *)p;

}


void memory_arena_free(void) {

    memset(arena.base, 0, arena.capacity);

    if (munmap(arena.base, arena.capacity) != 0) {
        
        perror("munmap");
        
        fprintf(stderr, "memory arena freeing failed\n");
        exit(EXIT_FAILURE);

    }

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