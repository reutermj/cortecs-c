#include <assert.h>
#include <common.h>
#include <cortecs/array.h>
#include <cortecs/finalizer.h>
#include <cortecs/gc.h>
#include <cortecs/log.h>
#include <cortecs/world.h>
#include <flecs.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

static cortecs_log_stream log_stream;

static cJSON *create_log_message(const char *method) {
    cJSON *message = cJSON_CreateObject();
    cJSON_AddStringToObject(message, "method", method);
    return message;
}

static void log_source_location(
    cJSON *message,
    const char *file,
    const char *function,
    int line
) {
    cJSON_AddStringToObject(message, "file", file);
    cJSON_AddStringToObject(message, "function", function);

    char buffer[sizeof("0xFFFF_FFFF_FFFF_FFFF")];
    snprintf(buffer, sizeof(buffer), "%d", line);
    cJSON_AddStringToObject(message, "line", buffer);
}

static void log_type_info(
    cJSON *message,
    cortecs_finalizer_index finalizer_index,
    bool is_array
) {
    cortecs_finalizer_metadata finalizer_metadata = cortecs_finalizer_get(finalizer_index);
    cJSON_AddStringToObject(message, "type_name", finalizer_metadata.type_name);
    cJSON_AddBoolToObject(message, "is_array", is_array);
}

static void log_allocation_info(
    cJSON *message,
    void *allocation,
    ecs_entity_t entity
) {
    char buffer[sizeof("0xFFFF_FFFF_FFFF_FFFF")];
    snprintf(buffer, sizeof(buffer), "0x%lx", (uintptr_t)allocation);
    cJSON_AddStringToObject(message, "pointer", buffer);

    snprintf(buffer, sizeof(buffer), "0x%lx", entity);
    cJSON_AddStringToObject(message, "entity_id", buffer);
}

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

#define ARRAY_BIT_ON (1 << 15)
#define ARRAY_BIT_OFF 0
#define ARRAY_BIT_CLEAR ~ARRAY_BIT_ON

// declared as a component, but it's really just an event.
// used to pass the allocation pointer to the dec observer
// without needing to know what size class the allocation is in
typedef struct {
    void *allocation;
    uint64_t event_id;
} dec;
static ECS_COMPONENT_DECLARE(dec);

static uint64_t dec_event_id;

static void perform_dec(
    void *allocation,
    const char *file,
    const char *function,
    int line,
    uint64_t event_id
) {
    ecs_entity_t entity = get_entity(allocation);
    gc_header *header = get_header(allocation);

    if (log_stream != NULL) {
        cJSON *message = create_log_message("cortecs_gc_dec");
        cJSON_AddStringToObject(message, "submethod", "perform_dec");
        if (file != NULL) {
            log_source_location(message, file, function, line);
        }

        if (event_id != 0) {
            char buffer[sizeof("0xFFFF_FFFF_FFFF_FFFF")];
            snprintf(buffer, sizeof(buffer), "0x%lx", event_id);
            cJSON_AddStringToObject(message, "event_id", "event_id");
        }

        log_type_info(
            message,
            header->type & ARRAY_BIT_CLEAR,
            (header->type & ARRAY_BIT_ON) == ARRAY_BIT_ON
        );
        log_allocation_info(message, allocation, header->entity);
        cortecs_log_write(log_stream, message);
        cJSON_Delete(message);
    }

    header->count--;
    if (header->count > 0) {
        return;
    }

    cortecs_finalizer_index index = header->type & ARRAY_BIT_CLEAR;
    if (!index) {
        goto delete_entity;
    }

    cortecs_finalizer_metadata type = cortecs_finalizer_get(index);
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
    perform_dec(event->allocation, NULL, NULL, 0, event->event_id);
    ecs_defer_resume(world);
}

void enqueue_dec(
    void *allocation,
    const char *file,
    const char *function,
    int line
) {
    ecs_entity_t entity = get_entity(allocation);
    uint64_t event_id = dec_event_id;
    dec_event_id++;

    if (log_stream != NULL) {
        gc_header *header = get_header(allocation);
        cJSON *message = create_log_message("cortecs_gc_inc");
        log_source_location(message, file, function, line);

        char buffer[sizeof("0xFFFF_FFFF_FFFF_FFFF")];
        snprintf(buffer, sizeof(buffer), "0x%lx", event_id);
        cJSON_AddStringToObject(message, "event_id", "event_id");

        log_type_info(
            message,
            header->type & ARRAY_BIT_CLEAR,
            (header->type & ARRAY_BIT_ON) == ARRAY_BIT_ON
        );
        log_allocation_info(message, allocation, header->entity);
        cortecs_log_write(log_stream, message);
        cJSON_Delete(message);
    }
    // need to use a component event so that the pointer
    // can be passed to the observer without knowing which
    // size class was used to allocate it
    ecs_event_desc_t dec_event = {
        .event = ecs_id(dec),
        .entity = entity,
        .param = &(dec){
            .allocation = allocation,
            .event_id = event_id,
        },
    };
    ecs_enqueue(world, &dec_event);
}

void cortecs_gc_dec_impl(
    void *allocation,
    const char *file,
    const char *function,
    int line
) {
    if (ecs_is_deferred(world)) {
        // a system is running.
        // Defer the decrement until after system logic completes
        enqueue_dec(allocation, file, function, line);
    } else {
        // called as a result of another allocation being collected
        // immediately perform the dec instead of deferring it
        perform_dec(allocation, file, function, line, 0);
    }
}

void cortecs_gc_init(cortecs_string log_path) {
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

    // initialize log stream
    dec_event_id = 1;
    if (log_path != NULL) {
        bool is_already_deferred = ecs_is_deferred(world);  // todo reconsider this part of the code
        if (!is_already_deferred) {
            ecs_defer_begin(world);
        }

        log_stream = cortecs_log_open(log_path);
        cortecs_gc_inc(log_stream);

        if (!is_already_deferred) {
            ecs_defer_end(world);
        }
    } else {
        log_stream = NULL;
    }
}

void *alloc(
    uint32_t size_of_allocation,
    cortecs_finalizer_index finalizer_index,
    uint16_t array_bit,
    const char *method,
    const char *file,
    const char *function,
    int line
) {
    ecs_entity_t entity = ecs_new(world);

    // first try to allocate from one of the size classes
    void *allocation;
    int size_class_index = 0;
    for (; size_class_index < CORTECS_GC_NUM_SIZES; size_class_index++) {
        if (size_of_allocation < buffer_sizes[size_class_index]) {
            allocation = ecs_emplace_id(world, entity, gc_buffers[size_class_index], NULL);
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

    if (log_stream != NULL) {
        cJSON *message = create_log_message(method);
        log_source_location(message, file, function, line);
        log_type_info(message, finalizer_index, array_bit == ARRAY_BIT_ON);
        log_allocation_info(message, out_pointer, entity);

        if (size_class_index == CORTECS_GC_NUM_SIZES) {
            cJSON_AddStringToObject(message, "size_class", "malloc");
        } else {
            char buffer[sizeof("0xFFFF_FFFF")];
            snprintf(buffer, sizeof(buffer), "%d", buffer_sizes[size_class_index]);
            cJSON_AddStringToObject(message, "size_class", buffer);
        }

        cortecs_log_write(log_stream, message);
        cJSON_Delete(message);
    }

    // defer a decrement to collect allocations that are never
    // attached to an entity
    enqueue_dec(out_pointer, file, function, line);

    return out_pointer;
}

void *cortecs_gc_alloc_impl(
    uint32_t size_of_type,
    cortecs_finalizer_index finalizer_index,
    const char *file,
    const char *function,
    int line
) {
    return alloc(
        size_of_type,
        finalizer_index,
        ARRAY_BIT_OFF,
        "cortecs_gc_alloc",
        file,
        function,
        line
    );
}

void *cortecs_gc_alloc_array_impl(
    uint32_t size_of_type,
    uint32_t size_of_array,
    uint32_t offset_of_elements,
    cortecs_finalizer_index finalizer_index,
    const char *file,
    const char *function,
    int line
) {
    void *allocation = alloc(
        size_of_type * size_of_array + offset_of_elements,
        finalizer_index,
        ARRAY_BIT_ON,
        "cortecs_gc_alloc_array",
        file,
        function,
        line
    );

    uint32_t *size = allocation;
    *size = size_of_array;

    return allocation;
}

void cortecs_gc_inc_impl(
    void *allocation,
    const char *file,
    const char *function,
    int line
) {
    gc_header *header = get_header(allocation);

    if (log_stream != NULL) {
        cJSON *message = create_log_message("cortecs_gc_inc");
        log_source_location(message, file, function, line);
        log_type_info(
            message,
            header->type & ARRAY_BIT_CLEAR,
            (header->type & ARRAY_BIT_ON) == ARRAY_BIT_ON
        );
        log_allocation_info(message, allocation, header->entity);
        cortecs_log_write(log_stream, message);
        cJSON_Delete(message);
    }

    // Immediate increment
    // TODO make atomic
    header->count++;
}

bool cortecs_gc_is_alive(void *allocation) {
    gc_header header = *(gc_header *)allocation;
    UNUSED(header);

    ecs_entity_t entity = get_entity(allocation);
    return ecs_is_alive(world, entity);
}
