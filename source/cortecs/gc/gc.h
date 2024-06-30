#ifndef CORTECS_GC_GC_H
#define CORTECS_GC_GC_H

#include <flecs.h>

typedef struct {
    void *memory;
    ecs_entity_t entity;
} cortecs_gc_allocation_t;

cortecs_gc_allocation_t cortecs_gc_alloc();
void cortecs_gc_init();
void cortecs_gc_add(ecs_entity_t entity, cortecs_gc_allocation_t memory);
void cortecs_gc_remove(ecs_entity_t entity, cortecs_gc_allocation_t memory);
void cortecs_gc_add_root(ecs_entity_t entity, cortecs_gc_allocation_t memory);
void cortecs_gc_remove_root(ecs_entity_t entity, cortecs_gc_allocation_t memory);

#endif