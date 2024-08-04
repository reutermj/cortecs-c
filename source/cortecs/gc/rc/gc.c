#include <assert.h>
#include <common.h>
#include <flecs.h>
#include <gc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <world.h>

// Implementation of a size segregated, deferred reference counting gc
// Allocations are made from a set of size classes or will fallback to
// malloc if the allocation fits in none of the size classes
// Collection is handled with immediate increments and deferred decrements.
// Since all increments happen before any decrement, if the reference count
// is ever 0, the allocation is garbage and can be collected.

// Current memory layout
// [entity | finalizer | reference count | data buffer]

#define ENTITY_OFFSET (0)
#define FINALIZER_OFFSET (sizeof(ecs_entity_t))
#define RC_OFFSET (FINALIZER_OFFSET + sizeof(cortecs_gc_finalizer))
#define DATA_OFFSET (RC_OFFSET + sizeof(uint32_t))
#define HEADER_SIZE DATA_OFFSET

#define CORTECS_GC_NUM_SIZES 5
static const uint32_t buffer_sizes[CORTECS_GC_NUM_SIZES] = {32, 64, 128, 256, 512};
static const uint32_t name_max_size = sizeof("gc_buffer_512");
static ecs_entity_t gc_buffers[CORTECS_GC_NUM_SIZES + 1];

// struct for the malloc based fallback
typedef struct {
    void *ptr;
} gc_buffer_ptr;

static ecs_entity_t get_entity(void *allocation) {
    return *(ecs_entity_t *)((uintptr_t)allocation - DATA_OFFSET + ENTITY_OFFSET);
}

static cortecs_gc_finalizer get_finalizer(void *allocation) {
    return *(cortecs_gc_finalizer *)((uintptr_t)allocation - DATA_OFFSET + FINALIZER_OFFSET);
}

static uint32_t *get_reference_count(void *allocation) {
    return (uint32_t *)((uintptr_t)allocation - DATA_OFFSET + RC_OFFSET);
}

// declared as a component, but it's really just an event.
// used to pass the allocation pointer to the dec observer
// without needing to know what size class the allocation is in
typedef struct {
    void *allocation;
} dec;
static ECS_COMPONENT_DECLARE(dec);

static void on_dec(ecs_iter_t *iterator) {
    assert(iterator->count == 1);
    dec *event = iterator->param;
    void *allocation = event->allocation;
    uint32_t *reference_count = get_reference_count(allocation);
    *reference_count = *reference_count - 1;
    if (*reference_count == 0) {
        cortecs_gc_finalizer finalizer = get_finalizer(allocation);
        if (finalizer) {
            finalizer(allocation);
        }
        ecs_delete(world, iterator->entities[0]);
    }
}

static void free_gc_buffer_ptr(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
    UNUSED(type_info);
    assert(count == 1);
    gc_buffer_ptr *buffer = (gc_buffer_ptr *)ptr;
    free(buffer->ptr);
}

void cortecs_gc_init() {
    // initialize the various size classes
    char name[name_max_size];
    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        uint32_t size = buffer_sizes[i];
        snprintf(name, name_max_size, "gc_buffer_%d", size);
        ecs_component_desc_t desc = {
            .entity = ecs_entity_init(world, &(ecs_entity_desc_t){.name = name}),
            .type = {
                .size = HEADER_SIZE + size,
                .alignment = HEADER_SIZE + size,
            },
        };
        gc_buffers[i] = ecs_component_init(world, &desc);
        // marking the buffer as sparse makes sure it doesnt get moved and
        // pointers to it are stable
        ecs_add_id(world, gc_buffers[i], EcsSparse);
    }

    // initialize the malloc based buffer
    // malloc based buffer doesnt need to be sparse
    ecs_component_desc_t desc = {
        .entity = ecs_entity_init(world, &(ecs_entity_desc_t){.name = "gc_buffer_ptr"}),
        .type = {
            .size = ECS_SIZEOF(gc_buffer_ptr),
            .alignment = ECS_ALIGNOF(gc_buffer_ptr),
            .hooks = {
                // hook to free the malloced pointer when deleting the entity
                .dtor = free_gc_buffer_ptr,
            },
        },
    };
    gc_buffers[CORTECS_GC_NUM_SIZES] = ecs_component_init(world, &desc);

    // initialize the dec event and observer
    ECS_COMPONENT_DEFINE(world, dec);
    ecs_observer_desc_t dec_desc = {
        .query = {
            .terms[0].id = EcsAny,
        },
        .events[0] = ecs_id(dec),
        .callback = on_dec,
    };
    ecs_observer_init(world, &dec_desc);
}

void gc_dec(ecs_entity_t target, void *allocation) {
    // need to use a component event so that the pointer
    // can be passed to the observer without knowing which
    // size class was used to allocate it
    ecs_event_desc_t dec_event = {
        .event = ecs_id(dec),
        .entity = target,
        .param = &(dec){.allocation = allocation},
    };
    ecs_enqueue(world, &dec_event);
}

static void *alloc(uint32_t size, ecs_entity_t entity, cortecs_gc_finalizer finalizer) {
    // first try to allocate from one of the size classes
    void *allocation;
    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        if (size < buffer_sizes[i]) {
            allocation = ecs_emplace_id(world, entity, gc_buffers[i], NULL);
            goto initialize_allocation;
        }
    }

    // fallback to malloc based buffer
    gc_buffer_ptr *buffer = ecs_emplace_id(world, entity, gc_buffers[CORTECS_GC_NUM_SIZES], NULL);
    allocation = malloc(HEADER_SIZE + size);
    buffer->ptr = allocation;

initialize_allocation:;
    ecs_entity_t *entity_location = allocation;
    *entity_location = entity;
    cortecs_gc_finalizer *finalizer_location = (cortecs_gc_finalizer *)((uintptr_t)allocation + FINALIZER_OFFSET);
    *finalizer_location = finalizer;
    uint32_t *count_location = (uint32_t *)((uintptr_t)allocation + RC_OFFSET);
    *count_location = 1;

    return (void *)((uintptr_t)count_location + sizeof(uint32_t));
}

void *cortecs_gc_alloc(uint32_t size, cortecs_gc_finalizer finalizer) {
    ecs_entity_t entity = ecs_new(world);
    void *allocation = alloc(size, entity, finalizer);

    // defer a decrement to collect allocations that are never
    // attached to an entity
    gc_dec(entity, allocation);
    return allocation;
}

void cortecs_gc_inc(void *allocation) {
    // Immediate increment
    // TODO make atomic
    uint32_t *count = get_reference_count(allocation);
    *count = *count + 1;
}

void cortecs_gc_dec(void *allocation) {
    // Deferred decrement
    gc_dec(get_entity(allocation), allocation);
}

bool cortecs_gc_is_alive(void *allocation) {
    ecs_entity_t entity = get_entity(allocation);
    return ecs_is_alive(world, entity);
}