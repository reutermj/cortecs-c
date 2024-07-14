#include <assert.h>
#include <common.h>
#include <gc.h>
#include <world.h>

// currently only allowing buffer size of 256. Will expand later
#define CORTECS_GC_ALLOC_MAX_SIZE 256
typedef struct {
    uint8_t data[CORTECS_GC_ALLOC_MAX_SIZE];
} gc_buffer;
static ECS_COMPONENT_DECLARE(gc_buffer);

// used to signify that an allocation in directly connected to a "live" entity
static ecs_entity_t root;

// used to signify that an allocation is connected to another dynamic allocation
static ecs_entity_t reachable;

// Basic tracing algorithm:
// Definitions:
// An allocation is rooted iff it is directly accessible to a component on some entity
// An allocation is reachable iff it is accessible to some other memory
// An allocation is root_reachable given the following rules:
//   1) A root_reachable B <- A root B
//   2) A root_reachable B <- A reachable C & C root_reachable B

// Iterate all not root reachable allocations and delete the entities

void run_gc(ecs_iter_t *iterator) {
    for (int i = 0; i < iterator->count; i++) {
        ecs_delete(world, iterator->entities[i]);
    }
}

void cortecs_gc_init() {
    ECS_COMPONENT_DEFINE(world, gc_buffer);
    // marking the buffer as sparse makes sure it doesnt get moved and
    // pointers to it are stable
    ecs_add_id(world, ecs_id(gc_buffer), EcsSparse);

    root = ecs_new(world);
    reachable = ecs_new(world);
    // the mark query traverses up reachable to find root relationships
    ecs_add_id(world, reachable, EcsTraversable);

    // define the mark system
    ecs_id_t run_on_post_frame[2] = {ecs_dependson(EcsPostFrame), 0};
    ecs_entity_desc_t mark_entity = {
        .name = "run_gc",
        .add = run_on_post_frame,
    };
    ecs_system_desc_t mark_desc = {
        .entity = ecs_entity_init(world, &mark_entity),
        .query = {
            .terms[0].first.id = ecs_id(gc_buffer),
            .terms[1] = {
                // select nodes where root appears either on EcsThis or by traversing up the reachable relationship
                .src.id = EcsSelf | EcsUp,
                .first.id = root,
                .second.id = EcsAny,  // Dont care about the target of the root relationship
                .trav = reachable,
                .oper = EcsNot,
            },
        },
        .callback = &run_gc,
    };
    ecs_system_init(world, &mark_desc);
}

cortecs_gc_allocation_t cortecs_gc_alloc(uint32_t size) {
    assert(size <= CORTECS_GC_ALLOC_MAX_SIZE);

    ecs_entity_t entity = ecs_new(world);
    void *memory = ecs_emplace_id(world, entity, ecs_id(gc_buffer), NULL);

    return (cortecs_gc_allocation_t){
        .memory = memory,
        .entity = entity,
    };
}

void cortecs_gc_add(cortecs_gc_allocation_t target, cortecs_gc_allocation_t reference) {
    ecs_add_pair(world, reference.entity, reachable, target.entity);
}

void cortecs_gc_remove(cortecs_gc_allocation_t target, cortecs_gc_allocation_t reference) {
    ecs_remove_pair(world, reference.entity, reachable, target.entity);
}

void cortecs_gc_add_root(ecs_entity_t target, cortecs_gc_allocation_t reference) {
    ecs_add_pair(world, reference.entity, root, target);
}

void cortecs_gc_remove_root(ecs_entity_t target, cortecs_gc_allocation_t reference) {
    ecs_remove_pair(world, reference.entity, root, target);
}
