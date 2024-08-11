#include <assert.h>
#include <common.h>
#include <flecs.h>
#include <gc.h>
#include <stdatomic.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <world.h>

// Allocations are made from a set of size classes or will fallback to
// malloc if the allocation fits in none of the size classes
// Collection is handled with immediate increments and deferred decrements.
// Since all increments happen before any decrement, if the reference count
// is ever 0, the allocation is garbage and can be collected.

// Ranges for the internal gc entities
#define ALLOCATION_ENTITY_MIN (CORTECS_APPLICATION_ENTITY_MAX + 1)
#define ALLOCATION_ENTITY_MAX UINT32_MAX

// Define the header bitfields and functions to get/set
#define HEADER_SIZE sizeof(_Atomic(ecs_entity_t))

#define TYPE_SHIFT 20
#define TYPE_BITS 12
#define TYPE_MASK MASK(TYPE_BITS, TYPE_SHIFT)
#define TYPE_MAX_VALUE MAX_VALUE(TYPE_BITS)
#define TYPE_CLEAR ~TYPE_MASK

static cortecs_gc_type_index get_type(ecs_entity_t header) {
    return (cortecs_gc_type_index)((header & TYPE_MASK) >> TYPE_SHIFT);
}

static ecs_entity_t set_type(ecs_entity_t header, cortecs_gc_type_index type) {
    assert(type <= TYPE_MAX_VALUE);
    return (header & TYPE_CLEAR) | ((ecs_entity_t)type << TYPE_SHIFT);
}

#define COUNT_SHIFT 48
#define COUNT_BITS 15
#define COUNT_MASK MASK(COUNT_BITS, COUNT_SHIFT)
#define COUNT_MAX_VALUE MAX_VALUE(COUNT_BITS)
#define COUNT_CLEAR ~COUNT_MASK
#define COUNT_ONE (1UL << COUNT_SHIFT)

// static uint16_t get_count(ecs_entity_t header) {
//     return (uint16_t)((header & COUNT_MASK) >> COUNT_SHIFT);
// }

static ecs_entity_t set_count(ecs_entity_t header, uint16_t count) {
    assert(count <= COUNT_MAX_VALUE);
    return (header & COUNT_CLEAR) | ((ecs_entity_t)count << COUNT_SHIFT);
}

#define ARRAY_FLAG (1 << 63)

// Define registered type info
// reserved types:
//   0: NOOP on collection
//   1: Decrement pointer on collection (mostly useful for arrays of pointers)
typedef struct {
    cortecs_gc_finalizer finalizer;
    uint32_t size;
} gc_type_info;
#define MAX_REGISTERED_TYPES (1 << TYPE_BITS)
static gc_type_info registered_types[MAX_REGISTERED_TYPES];
static uint32_t next_type_index = 2;

// define the size classes
#define CORTECS_GC_NUM_SIZES 5
static const uint32_t buffer_sizes[CORTECS_GC_NUM_SIZES] = {32, 64, 128, 256, 512};
static const uint32_t name_max_size = sizeof("gc_buffer_512");
static ecs_entity_t gc_buffers[CORTECS_GC_NUM_SIZES + 1];

// struct for the malloc based fallback
typedef struct {
    void *ptr;
} gc_buffer_ptr;

// used to pass the allocation pointer to the dec observer
// without needing to know what size class the allocation is in
typedef struct {
    void *allocation;
} dec;
static ECS_COMPONENT_DECLARE(dec);

static _Atomic(ecs_entity_t) *get_header(void *allocation) {
    return (_Atomic(ecs_entity_t) *)((uintptr_t)allocation - sizeof(ecs_entity_t));
}

static ecs_entity_t load_header(void *allocation) {
    _Atomic(ecs_entity_t) *header = get_header(allocation);
    return atomic_load_explicit(header, memory_order_relaxed);
}

static ecs_entity_t get_entity(void *allocation) {
    ecs_entity_t header = load_header(allocation);
    header = set_type(header, TYPE_MAX_VALUE);
    header = set_count(header, 0);
    return header;
}

static void on_dec(ecs_iter_t *iterator) {
    assert(iterator->count == 1);
    dec *event = iterator->param;
    void *allocation = event->allocation;
    _Atomic(ecs_entity_t) *header = get_header(allocation);
    ecs_entity_t entity = atomic_fetch_sub_explicit(header, COUNT_ONE, memory_order_relaxed);
    // atomic_fetch_sub returns the previous value, so we need to check for count == 1 not 0
    if ((entity & COUNT_MASK) == COUNT_ONE) {
        cortecs_gc_type_index type_index = get_type(entity);
        if (type_index) {
            gc_type_info type_info = registered_types[type_index];
            cortecs_gc_finalizer finalizer = type_info.finalizer;
            if (finalizer) {
                finalizer(allocation);
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
                .size = (ecs_size_t)(HEADER_SIZE + size),
                .alignment = (ecs_size_t)(HEADER_SIZE + size),
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

cortecs_gc_type_index cortecs_gc_register_type(cortecs_gc_finalizer finalizer, uint32_t size) {
    uint32_t index = next_type_index;
    next_type_index++;

    registered_types[index] = (gc_type_info){
        .finalizer = finalizer,
        .size = size,
    };

    return index;
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
    allocation = malloc(HEADER_SIZE + element_size);
    buffer->ptr = allocation;

initialize_allocation:;
    _Atomic(ecs_entity_t) *header = allocation;
    entity = set_type(entity, type_index);
    entity = set_count(entity, 1);
    atomic_store_explicit(header, entity, memory_order_relaxed);
    return (void *)((uintptr_t)allocation + HEADER_SIZE);
}

void *cortecs_gc_alloc(uint32_t element_size, cortecs_gc_type_index type_index) {
    ecs_set_entity_range(world, ALLOCATION_ENTITY_MIN, ALLOCATION_ENTITY_MAX);
    ecs_entity_t entity = ecs_new(world);
    ecs_set_entity_range(world, CORTECS_APPLICATION_ENTITY_MIN, CORTECS_APPLICATION_ENTITY_MAX);
    void *allocation = alloc(element_size, entity, type_index);

    // defer a decrement to collect allocations that are never
    // attached to an entity
    gc_dec(entity, allocation);

    return allocation;
}

void cortecs_gc_inc(void *allocation) {
    // Immediate increment
    // Bug: currently this fails if some allocations has more that 2^COUNT_BITS references
    atomic_fetch_add_explicit(get_header(allocation), COUNT_ONE, memory_order_relaxed);
}

void cortecs_gc_dec(void *allocation) {
    // Deferred decrement
    // get_entity requires an atomic load, so it's possible
    // i can check for count overflow and stop emitting decs
    gc_dec(get_entity(allocation), allocation);
}

bool cortecs_gc_is_alive(void *allocation) {
    ecs_entity_t entity = get_entity(allocation);
    return ecs_is_alive(world, entity);
}
