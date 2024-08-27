#ifndef CORTECS_ARENA_ARENA_H
#define CORTECS_ARENA_ARENA_H

#include <stdbool.h>
#include <stdint.h>

void cortecs_arena_init();
void cortecs_arena_reset();
void *cortecs_arena_alloc(uint32_t size_of_element);
void *cortecs_arena_alloc_array(uint32_t size_of_elements, uint32_t size_of_array);

#endif