#include <assert.h>
#include <gc.h>
#include <world.h>

// High level overview of memory allocations in cortecs
// * static size: size of the allocation can be determined at compile time
// * dynamic size: size of the allocation depends on some runtime value
// Currently, the only way for an allocation to escape a system is for
// it to be set on a component.
// * non-escaping allocations never invoke the gc
// * static non-escaping: stack allocated
// * dynamic non-escaping: allocated on a per thread arena allocator
// * escaping static allocations
//   * set the component value
//   * gc is not involved
// * escaping dynamic allocation
//   * gc has to be involved

// Application entities: entities created by the application
// Allocation entities: entities created by the gc to track dynamic allocation liveness
// The tracing algorithm introduces two relationships to track liveness
//   * A rooted_to B: a relationship from allocation entity A to an application entity B
//     * In tracing gc terminology, A is part of the root set
//     * Relationships are used so that if the application deletes B,
//       the A rooted_to B relationship is automatically removed
//   * A reachable_to B: a relationship from allocation entity A to another allocation entity B
// These relationships are updated by codegened API calls and application entity deletion

// The tracing algorithm introduces a derivative relationship: root_reachable_to.
// An allocation entity A is root_reachable_to an application entity B given the following rules:
//   1) A root_reachable_to B <- A rooted_to B
//   2) A root_reachable_to B <- there exists an allocation entity C such that A reachable_to C & C root_reachable_to B

// The tracing algorithm introduces a derivative property: root_reachable
// An allocation entity A is root_reachable iff there exists an application entity B such that A root_reachable_to B

// The tracing algorithm: For all A where A is not root_reachable, delete A

// currently only allowing buffer size of 256. Will expand later
#define CORTECS_GC_ALLOC_MAX_SIZE 256
typedef struct {
    ecs_entity_t entity;
    uint8_t data[CORTECS_GC_ALLOC_MAX_SIZE];
} gc_buffer;
static ECS_COMPONENT_DECLARE(gc_buffer);

static ecs_entity_t rooted_to;
static ecs_entity_t reachable_to;

static void delete_not_root_reachable_allocations(ecs_iter_t *iterator) {
    for (int i = 0; i < iterator->count; i++) {
        ecs_delete(world, iterator->entities[i]);
    }
}

void cortecs_gc_init() {
    // marking the buffer as sparse makes pointers to it stable
    ECS_COMPONENT_DEFINE(world, gc_buffer);
    ecs_add_id(world, ecs_id(gc_buffer), EcsSparse);

    rooted_to = ecs_new(world);
    reachable_to = ecs_new(world);
    ecs_add_id(world, reachable_to, EcsTraversable);

    ecs_query_desc_t is_not_root_reachable = {
        .terms[0].first.id = ecs_id(gc_buffer),
        .terms[1] = {
            .src.id = EcsSelf | EcsUp,
            .first.id = rooted_to,
            .second.id = EcsAny,
            .trav = reachable_to,
            .oper = EcsNot,
        },
    };

    // init tracing system
    ecs_entity_desc_t tracing_entity = {
        .name = "tracing_gc",
        .add = ecs_ids(ecs_dependson(EcsPostFrame)),
    };
    ecs_system_desc_t tracing_system = {
        .entity = ecs_entity_init(world, &tracing_entity),
        .query = is_not_root_reachable,
        .callback = &delete_not_root_reachable_allocations,
    };
    ecs_system_init(world, &tracing_system);
}

void *cortecs_gc_alloc(uint32_t size) {
    assert(size <= CORTECS_GC_ALLOC_MAX_SIZE);

    ecs_entity_t entity = ecs_new(world);
    gc_buffer *allocation = ecs_emplace_id(world, entity, ecs_id(gc_buffer), NULL);
    allocation->entity = entity;

    return allocation->data;
}

ecs_entity_t get_allocation_entity(void *allocation) {
    gc_buffer *buffer = (gc_buffer *)(((uintptr_t)allocation) - sizeof(ecs_entity_t));
    return buffer->entity;
}

bool cortecs_gc_is_alive(void *target) {
    ecs_entity_t entity = get_allocation_entity(target);
    return ecs_is_alive(world, entity);
}

void cortecs_gc_add(void *target, void *reference) {
    ecs_entity_t target_entity = get_allocation_entity(target);
    ecs_entity_t reference_entity = get_allocation_entity(reference);
    ecs_add_pair(world, reference_entity, reachable_to, target_entity);
}

void cortecs_gc_remove(void *target, void *reference) {
    ecs_entity_t target_entity = get_allocation_entity(target);
    ecs_entity_t reference_entity = get_allocation_entity(reference);
    ecs_remove_pair(world, reference_entity, reachable_to, target_entity);
}

void cortecs_gc_add_root(ecs_entity_t target, void *reference) {
    ecs_entity_t reference_entity = get_allocation_entity(reference);
    ecs_add_pair(world, reference_entity, rooted_to, target);
}

void cortecs_gc_remove_root(ecs_entity_t target, void *reference) {
    ecs_entity_t reference_entity = get_allocation_entity(reference);
    ecs_remove_pair(world, reference_entity, rooted_to, target);
}
