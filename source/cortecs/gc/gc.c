#include <assert.h>
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

static gc_header *get_header(void *allocation) {
    return (gc_header *)((uintptr_t)allocation - sizeof(gc_header));
}

static ecs_entity_t get_entity(void *allocation) {
    return get_header(allocation)->entity;
}

#define CORTECS_GC_NUM_SIZES 5
static const uint32_t buffer_sizes[CORTECS_GC_NUM_SIZES] = {32, 64, 128, 256, 512};
static const uint32_t name_max_size = sizeof("gc_buffer_512");
static ecs_entity_t gc_buffers[CORTECS_GC_NUM_SIZES + 1];

// struct for the malloc based fallback
typedef struct {
    void *ptr;
} gc_buffer_ptr;

static void free_gc_buffer_ptr(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
    UNUSED(type_info);
    assert(count == 1);
    gc_buffer_ptr *buffer = (gc_buffer_ptr *)ptr;
    free(buffer->ptr);
}

typedef struct {
    cortecs_gc_finalizer_type finalizer;
    uintptr_t size;
    uintptr_t offset_of_elements;
} gc_type_info;

// Define registered type info
// reserved types:
//   0: NOOP on collection
//   1: Decrement pointer on collection (mostly useful for arrays of pointers)
#define TYPE_BITS 16
#define MAX_REGISTERED_TYPES (1 << TYPE_BITS)
static gc_type_info registered_types[MAX_REGISTERED_TYPES];
static uint32_t next_type_index;

#define ARRAY_BIT_ON (1 << 15)
#define ARRAY_BIT_OFF 0
#define ARRAY_BIT_CLEAR ~ARRAY_BIT_ON

cortecs_gc_finalizer_index cortecs_gc_register_finalizer(cortecs_gc_finalizer_type finalizer, uintptr_t size, uintptr_t offset_of_elements) {
    cortecs_gc_finalizer_index index = next_type_index;
    next_type_index++;
    registered_types[index] = (gc_type_info){
        .finalizer = finalizer,
        .size = size,
        .offset_of_elements = offset_of_elements,
    };
    return index;
}

static void cleanup_pointer(void *allocation) {
    cortecs_gc_dec(*(void **)allocation);
}

// declared as a component, but it's really just an event.
// used to pass the allocation pointer to the dec observer
// without needing to know what size class the allocation is in
typedef struct {
    void *allocation;
} dec;
static ECS_COMPONENT_DECLARE(dec);

static void perform_dec(ecs_entity_t entity, void *allocation) {
    gc_header *header = get_header(allocation);
    header->count--;
    if (header->count > 0) {
        return;
    }

    cortecs_gc_finalizer_index index = header->type & ARRAY_BIT_CLEAR;
    if (!index) {
        goto delete_entity;
    }

    gc_type_info type = registered_types[index];
    if (header->type & ARRAY_BIT_ON) {
        uint32_t size_of_array = *(uint32_t *)allocation;
        uintptr_t base = (uintptr_t)allocation + type.offset_of_elements;
        uintptr_t upper_bound = base + size_of_array * type.size;
        for (uintptr_t element = base; element < upper_bound; element += type.size) {
            type.finalizer((void *)element);
        }
    } else {
        type.finalizer(allocation);
    }

delete_entity:;
    // this must go after finalization because collection happens eagerly
    // and flecs may overwrite the data before calling the finalizer
    ecs_delete(world, entity);
}

static void dec_event_handler(ecs_iter_t *iterator) {
    assert(iterator->count == 1);
    dec *event = iterator->param;
    // suspend deferring so that recursive decs are immediately processed
    ecs_defer_suspend(world);
    perform_dec(iterator->entities[0], event->allocation);
    ecs_defer_resume(world);
}

void enqueue_dec(ecs_entity_t entity, void *allocation) {
    // need to use a component event so that the pointer
    // can be passed to the observer without knowing which
    // size class was used to allocate it
    ecs_event_desc_t dec_event = {
        .event = ecs_id(dec),
        .entity = entity,
        .param = &(dec){.allocation = allocation},
    };
    ecs_enqueue(world, &dec_event);
}

void cortecs_gc_dec(void *allocation) {
    ecs_entity_t entity = get_entity(allocation);
    if (ecs_is_deferred(world)) {
        // a system is running.
        // Defer the decrement until after system logic completes
        enqueue_dec(entity, allocation);
    } else {
        // called as a result of another allocation being collected
        // immediately perform the dec instead of deferring it
        perform_dec(entity, allocation);
    }
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
        .callback = dec_event_handler,
    };
    ecs_observer_init(world, &dec_desc);

    next_type_index = 2;
    registered_types[1] = (gc_type_info){
        .finalizer = cleanup_pointer,
        .size = sizeof(void *),
    };
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
    enqueue_dec(entity, out_pointer);

    return out_pointer;
}

void *cortecs_gc_alloc_impl(uint32_t size_of_type, cortecs_gc_finalizer_index finalizer_index) {
    return alloc(size_of_type, finalizer_index, ARRAY_BIT_OFF);
}

void *cortecs_gc_alloc_array_impl(uint32_t size_of_type, uint32_t size_of_array, uint32_t offset_of_elements, cortecs_gc_finalizer_index finalizer_index) {
    // offset_of_elements points to the variable length elements array in the cortecs_array_TYPE struct
    // based on alignment of different types, the offset of the elements array may not be the same in
    // both cortecs_array_uint32 and cortecs_array_uint64, and C compilers may add different amounts.
    // of padding to both. We pass the offset of this element into this allocator to correctly allocate
    // the right amount of space for the specific array type.

    void *allocation = alloc(
        size_of_type * size_of_array + offset_of_elements,
        finalizer_index,
        ARRAY_BIT_ON
    );

    uint32_t *size = allocation;
    *size = size_of_array;

    return allocation;
}

void cortecs_gc_inc(void *allocation) {
    // Immediate increment
    // TODO make atomic
    get_header(allocation)->count++;
}

bool cortecs_gc_is_alive(void *allocation) {
    ecs_entity_t entity = get_entity(allocation);
    return ecs_is_alive(world, entity);
}
