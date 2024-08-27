#include <cortecs/arena.h>
#include <cortecs/array.h>
#include <stdlib.h>

static uintptr_t arena_base;
static uintptr_t arena_offset;

#define ARENA_SIZE (4UL * 1024UL * 1024UL)

void cortecs_arena_init() {
    arena_base = (uintptr_t)malloc(ARENA_SIZE);
    arena_offset = 0;
}

void cortecs_arena_reset() {
    arena_offset = 0;
}

void *cortecs_arena_alloc(uint32_t size_of_element) {
    void *allocation = (void *)arena_base;
    arena_offset += size_of_element;  // todo. might want to word size align this
    return allocation;
}

void *cortecs_arena_alloc_array(uint32_t size_of_elements, uint32_t size_of_array) {
    cortecs_array(void) allocation = cortecs_arena_alloc(
        size_of_elements * size_of_array + (uint32_t)sizeof(uint32_t)
    );
    allocation->size = size_of_array;
    return allocation;
}