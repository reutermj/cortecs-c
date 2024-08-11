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

typedef struct {
    ecs_entity_t entity;
    uint16_t type;
    uint16_t count;
} gc_header;

#define ENTITY_OFFSET (0)
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

static gc_header *get_header(void *allocation) {
    return (gc_header *)((uintptr_t)allocation - sizeof(gc_header));
}

static ecs_entity_t get_entity(void *allocation) {
    return get_header(allocation)->entity;
}

typedef struct {
    cortecs_gc_finalizer finalizer;
    uint32_t size;
} gc_type_info;

// Define registered type info
// reserved types:
//   0: NOOP on collection
//   1: Decrement pointer on collection (mostly useful for arrays of pointers)
#define TYPE_BITS 16
#define MAX_REGISTERED_TYPES (1 << TYPE_BITS)
static gc_type_info registered_types[MAX_REGISTERED_TYPES];
static uint32_t next_type_index = 2;

cortecs_gc_type_index cortecs_gc_register_type(cortecs_gc_finalizer finalizer, uint32_t size) {
    cortecs_gc_type_index index = next_type_index;
    next_type_index++;
    registered_types[index] = (gc_type_info){
        .finalizer = finalizer,
        .size = size,
    };
    return index;
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
    gc_header *header = get_header(allocation);
    header->count--;
    if (header->count == 0) {
        if (header->type) {
            gc_type_info type = registered_types[header->type];
            if (type.finalizer) {
                type.finalizer(allocation);
            }
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
                .size = (ecs_size_t)(sizeof(gc_header) + size),
                .alignment = (ecs_size_t)(sizeof(gc_header) + size),
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

static void *alloc(uint32_t element_size, ecs_entity_t entity, cortecs_gc_type_index type_index) {
    // first try to allocate from one of the size classes
    void *allocation;
    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        if (element_size < buffer_sizes[i]) {
            allocation = ecs_emplace_id(world, entity, gc_buffers[i], NULL);
            goto initialize_allocation;
        }
    }

    // fallback to malloc based buffer
    gc_buffer_ptr *buffer = ecs_emplace_id(world, entity, gc_buffers[CORTECS_GC_NUM_SIZES], NULL);
    allocation = malloc(sizeof(gc_header) + element_size);
    buffer->ptr = allocation;

initialize_allocation:;
    gc_header *header = allocation;
    header->entity = entity;
    header->type = type_index;
    header->count = 1;

    return (void *)((uintptr_t)allocation + sizeof(gc_header));
}

void *cortecs_gc_alloc(uint32_t element_size, cortecs_gc_type_index type_index) {
    ecs_entity_t entity = ecs_new(world);
    void *allocation = alloc(element_size, entity, type_index);

    // defer a decrement to collect allocations that are never
    // attached to an entity
    gc_dec(entity, allocation);
    return allocation;
}

void cortecs_gc_inc(void *allocation) {
    // Immediate increment
    // TODO make atomic
    get_header(allocation)->count++;
}

void cortecs_gc_dec(void *allocation) {
    // Deferred decrement
    gc_dec(get_entity(allocation), allocation);
}

bool cortecs_gc_is_alive(void *allocation) {
    ecs_entity_t entity = get_entity(allocation);
    return ecs_is_alive(world, entity);
}
