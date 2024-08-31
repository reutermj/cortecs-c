#include <assert.h>
#include <common.h>
#include <cortecs/array.h>
#include <cortecs/gc.h>
#include <cortecs/world.h>
#include <flecs.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

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

#define ARRAY_BIT_ON (1 << 15)
#define ARRAY_BIT_OFF 0
#define ARRAY_BIT_CLEAR ~ARRAY_BIT_ON

cortecs_gc_finalizer_index cortecs_gc_register_finalizer(cortecs_gc_finalizer finalizer, uint32_t size) {
    cortecs_gc_finalizer_index index = next_type_index;
    next_type_index++;
    registered_types[index] = (gc_type_info){
        .finalizer = finalizer,
        .size = size,
    };
    return index;
}

// the target of dec events
static ecs_entity_t dec_target;
typedef struct {
    // currently contains nothing simply because I need a component
    // to attach to the dec_target, but can be used later for tracking
    // gc calls or something
    uint32_t unused;
} dec_target_info;
static ECS_COMPONENT_DECLARE(dec_target_info);

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
            gc_type_info type = registered_types[header->type & ARRAY_BIT_CLEAR];
            if (type.finalizer) {
                if (header->type & ARRAY_BIT_ON) {
                    uint32_t size = *(uint32_t *)allocation;
                    void *current = (void *)((uintptr_t)allocation + sizeof(uint32_t));
                    for (uint32_t i = 0; i < size; i++) {
                        type.finalizer(current);
                        current = (void *)((uintptr_t)current + type.size);
                    }
                } else {
                    type.finalizer(allocation);
                }
            }
        }

        ecs_delete(world, get_entity(allocation));
    }
}

static void free_gc_buffer_ptr(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
    UNUSED(type_info);
    assert(count == 1);
    gc_buffer_ptr *buffer = (gc_buffer_ptr *)ptr;
    free(buffer->ptr);
}

void cortecs_gc_init() {
    // initialize entity to be the target of dec events
    // in flecs, events must be emitted on an entity that has components
    // if dec events are enqueued on the allocation entity, there's edge cases
    // that can lead to memory leaks:
    // 1) allocate a1 (enqueue a dec on a1)
    // 2) allocate a2 (enqueue a dec on a2)
    // 3) a2 is referenced by a1 (eagerly inc a2)
    // 4) a1 is never inced
    // at the end of the deferred context the command queue is the following:
    // Command Queue 0: Add size class to a1, Dec on a1, Add size class to a2, Dec on a2
    // refernce counts are the following:
    // a1: 1; a2: 2
    // 5) process (Add size class to a1)
    //   5.1) a1 has a component so a1 can be the target of an event
    //   5.1) a2 dosnt have a component so a2 can't be the target of an event
    // 6) process Dec on a1
    //   6.1) decrement the reference count on a1: 1 -> 0
    //   6.2) enqueue a dec on a2
    //     6.2.1) this create a new Command Queue 1 with only the (Dec on a2) event
    // 7) process Command Queue 1
    //   7.1) event (Add size class to a2) in Command Queue 0 still hasnt been processed,
    //        so a2 still has no components
    //   7.2) process (Dec on a2). a2 has no components, so this event is a noop
    // 8) finish processing Command Queue 0:
    //   8.1) process (Add size class to a2)
    //   8.2) process (Dec on a2) decremening the reference count on a2: 2 -> 1
    //     8.2.1) memory leak
    ECS_COMPONENT_DEFINE(world, dec_target_info);
    dec_target = ecs_new(world);
    ecs_add(world, dec_target, dec_target_info);

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

void gc_dec(void *allocation) {
    // need to use a component event so that the pointer
    // can be passed to the observer without knowing which
    // size class was used to allocate it
    ecs_event_desc_t dec_event = {
        .event = ecs_id(dec),
        .entity = dec_target,
        .param = &(dec){.allocation = allocation},
    };
    ecs_enqueue(world, &dec_event);
}

static void *alloc(uint32_t size_of_allocation, cortecs_gc_finalizer_index finalizer_index, uint16_t array_bit) {
    ecs_entity_t entity = ecs_new(world);

    // first try to allocate from one of the size classes
    void *allocation;
    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        if (size_of_allocation < buffer_sizes[i]) {
            allocation = ecs_emplace_id(world, entity, gc_buffers[i], NULL);
            goto initialize_allocation;
        }
    }

    // fallback to malloc based buffer
    gc_buffer_ptr *buffer = ecs_emplace_id(world, entity, gc_buffers[CORTECS_GC_NUM_SIZES], NULL);
    allocation = malloc(sizeof(gc_header) + size_of_allocation);
    buffer->ptr = allocation;

initialize_allocation:;
    gc_header *header = allocation;
    header->entity = entity;
    header->type = finalizer_index | array_bit;
    header->count = 1;

    void *out_pointer = (void *)((uintptr_t)allocation + sizeof(gc_header));

    // defer a decrement to collect allocations that are never
    // attached to an entity
    gc_dec(out_pointer);

    return out_pointer;
}

void *cortecs_gc_alloc(uint32_t size, cortecs_gc_finalizer_index finalizer_index) {
    return alloc(size, finalizer_index, ARRAY_BIT_OFF);
}

void *cortecs_gc_alloc_array(uint32_t size_of_elements, uint32_t size_of_array, cortecs_gc_finalizer_index finalizer_index) {
    cortecs_array(void) allocation = alloc(
        size_of_elements * size_of_array + (uint32_t)sizeof(uint32_t),
        finalizer_index,
        ARRAY_BIT_ON
    );
    allocation->size = size_of_array;
    return allocation;
}

void cortecs_gc_inc(void *allocation) {
    // Immediate increment
    // TODO make atomic
    get_header(allocation)->count++;
}

void cortecs_gc_dec(void *allocation) {
    // Deferred decrement
    gc_dec(allocation);
}

bool cortecs_gc_is_alive(void *allocation) {
    ecs_entity_t entity = get_entity(allocation);
    return ecs_is_alive(world, entity);
}
