#ifndef CORTECS_GC_GC_H
#define CORTECS_GC_GC_H

#include <flecs.h>
#include <stdint.h>

typedef struct {
    void *memory;
    ecs_entity_t entity;
} cortecs_gc_allocation_t;

cortecs_gc_allocation_t cortecs_gc_alloc(uint32_t size);
void cortecs_gc_init();
void cortecs_gc_add(cortecs_gc_allocation_t target, cortecs_gc_allocation_t reference);
void cortecs_gc_remove(cortecs_gc_allocation_t target, cortecs_gc_allocation_t reference);
void cortecs_gc_add_root(ecs_entity_t target, cortecs_gc_allocation_t reference);
void cortecs_gc_remove_root(ecs_entity_t target, cortecs_gc_allocation_t reference);
void cortecs_gc_memory_unused(cortecs_gc_allocation_t reference);

#endif