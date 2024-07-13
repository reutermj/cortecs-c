#include <assert.h>
#include <gc.h>
#include <world.h>

// currently only allowing buffer size of 256. Will expand later
#define CORTECS_GC_ALLOC_MAX_SIZE 256
typedef struct {
    uint8_t data[CORTECS_GC_ALLOC_MAX_SIZE];
} gc_buffer;
static ECS_COMPONENT_DECLARE(gc_buffer);

typedef struct {
    uint32_t count;
} reference_count;
static ECS_COMPONENT_DECLARE(reference_count);

static ecs_entity_t referenced_by;

static void on_inc(ecs_iter_t *iterator) {
    reference_count *count = ecs_field(iterator, reference_count, 1);
    for (int i = 0; i < iterator->count; i++) {
        count->count++;
    }
}

static void on_dec(ecs_iter_t *iterator) {
    reference_count *count = ecs_field(iterator, reference_count, 1);
    for (int i = 0; i < iterator->count; i++) {
        count->count--;
        if (count->count == 0) {
            ecs_delete(world, iterator->entities[i]);
        }
    }
}

static void remove_unused(ecs_iter_t *iterator) {
    for (int i = 0; i < iterator->count; i++) {
        ecs_delete(world, iterator->entities[i]);
    }
}

void cortecs_gc_init() {
    referenced_by = ecs_new(world);
    ECS_COMPONENT_DEFINE(world, reference_count);
    ECS_COMPONENT_DEFINE(world, gc_buffer);
    // marking the buffer as sparse makes sure it doesnt get moved and
    // pointers to it are stable
    ecs_add_id(world, ecs_id(gc_buffer), EcsSparse);

    ecs_observer_desc_t inc_desc = {
        .query = {
            .terms[0] = {
                .first.id = referenced_by,
                .second.id = EcsWildcard,
            },
            .terms[1].id = ecs_id(reference_count),
        },
        .events[0] = EcsOnAdd,
        .callback = on_inc,
    };
    ecs_observer_init(world, &inc_desc);

    ecs_observer_desc_t dec_desc = {
        .query = {
            .terms[0] = {
                .first.id = referenced_by,
                .second.id = EcsWildcard,
            },
            .terms[1].id = ecs_id(reference_count),
        },
        .events[0] = EcsOnRemove,
        .callback = on_dec,
    };
    ecs_observer_init(world, &dec_desc);

    ecs_id_t run_on_post_frame[2] = {ecs_dependson(EcsPostFrame), 0};
    ecs_entity_desc_t unused_memory_system = {
        .name = "remove_unused_allocations",
        .add = run_on_post_frame,
    };

    ecs_system_desc_t unused_desc = {
        .entity = ecs_entity_init(world, &unused_memory_system),
        .query = {
            .terms[0].first.id = ecs_id(reference_count),
            .terms[1] = {
                .first.id = referenced_by,
                .second.id = EcsWildcard,
                .oper = EcsNot,
            },
        },
        .callback = &remove_unused,
    };
    ecs_system_init(world, &unused_desc);
}

cortecs_gc_allocation_t cortecs_gc_alloc(uint32_t size) {
    assert(size <= CORTECS_GC_ALLOC_MAX_SIZE);

    ecs_entity_t entity = ecs_new(world);
    void *memory = ecs_emplace_id(world, entity, ecs_id(gc_buffer), NULL);
    ecs_set(world, entity, reference_count, {.count = 0});

    return (cortecs_gc_allocation_t){
        .memory = memory,
        .entity = entity,
    };
}

void cortecs_gc_add_root(ecs_entity_t target, cortecs_gc_allocation_t reference) {
    ecs_add_pair(world, reference.entity, referenced_by, target);
}

void cortecs_gc_remove_root(ecs_entity_t target, cortecs_gc_allocation_t reference) {
    ecs_remove_pair(world, reference.entity, referenced_by, target);
}

void cortecs_gc_add(cortecs_gc_allocation_t target, cortecs_gc_allocation_t reference) {
    cortecs_gc_add_root(target.entity, reference);
}

void cortecs_gc_remove(cortecs_gc_allocation_t target, cortecs_gc_allocation_t reference) {
    cortecs_gc_remove_root(target.entity, reference);
}
