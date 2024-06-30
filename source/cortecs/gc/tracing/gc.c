#include <assert.h>
#include <gc.h>
#include <stdint.h>
#include <stdio.h>
#include <world.h>

// An allocation is rooted iff it is directly accessible to a component on some entity
// An allocation is reachable iff it is accessible to some other memory

// An allocation is root_reachable given the following rules:
// A root_reachable B <- A root B
// A root_reachable B <- A reachable C & C root_reachable B

// Basic mark-sweep algorithm:
// 1) iterate all root_reachable memory and set the mark bit
// 2) iterate all allocations
//    2.1) delete nodes that arent root_reachable
//    2.2) reset the mark bit on nodes that are

#define CORTECS_GC_ALLOC_MAX_SIZE 256

typedef struct {
    uint8_t data[CORTECS_GC_ALLOC_MAX_SIZE];
} gc_buffer;
static ECS_COMPONENT_DECLARE(gc_buffer);

typedef struct {
    bool is_root_reachable;
} mark_sweep_data;
static ECS_COMPONENT_DECLARE(mark_sweep_data);

static ecs_entity_t root;
static ecs_entity_t reachable;

void mark(ecs_iter_t *iterator) {
    mark_sweep_data *data = ecs_field(iterator, mark_sweep_data, 0);
    for (int i = 0; i < iterator->count; i++) {
        data[i].is_root_reachable = true;
    }
}

void sweep(ecs_iter_t *iterator) {
    mark_sweep_data *data = ecs_field(iterator, mark_sweep_data, 0);
    for (int i = 0; i < iterator->count; i++) {
        if (!data[i].is_root_reachable) {
            ecs_delete(world, iterator->entities[i]);
        } else {
            data[i].is_root_reachable = false;
        }
    }
}

void cortecs_gc_init() {
    ECS_COMPONENT_DEFINE(world, gc_buffer);
    ECS_COMPONENT_DEFINE(world, mark_sweep_data);
    root = ecs_new(world);
    reachable = ecs_new(world);
    ecs_add_id(world, reachable, EcsTraversable);

    ecs_id_t run_on_post_frame[2] = {ecs_dependson(EcsPostFrame), 0};
    ecs_entity_desc_t mark_entity = (ecs_entity_desc_t){
        .name = "Mark",
        .add = run_on_post_frame,
    };
    ecs_system_desc_t mark_desc = (ecs_system_desc_t){
        .entity = ecs_entity_init(world, &mark_entity),
        .query = (ecs_query_desc_t){
            .terms[0].first.id = ecs_id(mark_sweep_data),
            .terms[1] = (ecs_term_t){
                .src.id = EcsSelf | EcsUp,
                .first.id = root,
                .second.name = "$Unused",
                .trav = reachable,
            },
        },
        .callback = &mark,
    };
    ecs_system_init(world, &mark_desc);

    ECS_SYSTEM(world, sweep, EcsPostFrame, [in] mark_sweep_data);
}

cortecs_gc_allocation_t cortecs_gc_alloc() {
    ecs_entity_t entity = ecs_new(world);
    ecs_add(world, entity, gc_buffer);
    ecs_set(world, entity, mark_sweep_data, {.is_root_reachable = false});
    return (cortecs_gc_allocation_t){
        .memory = ecs_get_mut(world, entity, gc_buffer),
        .entity = entity,
    };
}

void cortecs_gc_add(ecs_entity_t entity, cortecs_gc_allocation_t memory) {
    ecs_add_pair(world, memory.entity, reachable, entity);
}

void cortecs_gc_remove(ecs_entity_t entity, cortecs_gc_allocation_t memory) {
    ecs_remove_pair(world, memory.entity, reachable, entity);
}

void cortecs_gc_add_root(ecs_entity_t entity, cortecs_gc_allocation_t memory) {
    ecs_add_pair(world, memory.entity, root, entity);
}

void cortecs_gc_remove_root(ecs_entity_t entity, cortecs_gc_allocation_t memory) {
    ecs_remove_pair(world, memory.entity, root, entity);
}
