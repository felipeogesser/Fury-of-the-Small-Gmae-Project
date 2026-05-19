#ifndef MEMORY_ARENA_H
#define MEMORY_ARENA_H

#include <stddef.h>

void memory_arena_init(void);

void memory_arena_reset(void);

void memory_arena_free(void);

char *memory_arena_current_pointer(void);

size_t memory_arena_memory_used(void);

size_t memory_arena_memory_remainder(void);

size_t memory_arena_memory_capacity(void);

void *memory_arena_push(size_t size, size_t alignment);

#endif