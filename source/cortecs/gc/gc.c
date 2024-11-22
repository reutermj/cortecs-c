#include <assert.h>
#include <common.h>
#include <cortecs/finalizer.h>
#include <cortecs/gc.h>
#include <cortecs/log.h>
#include <cortecs/string.h>
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

// ====================================================================================================================
// Allocation Header
// ====================================================================================================================
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
#define NAME_MAX_SIZE sizeof("gc_buffer_512")
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

// ====================================================================================================================
// Logging
// ====================================================================================================================
static CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) log_stream;

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

    char buffer[sizeof("2,147,483,647")];
    snprintf(buffer, sizeof(buffer), "%" PRId32, line);
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
    snprintf(buffer, sizeof(buffer), "0x%" PRIxPTR, (uintptr_t)allocation);
    cJSON_AddStringToObject(message, "pointer", buffer);

    // only 32 bits. won't overflow buffer
    snprintf(buffer, sizeof(buffer), "%" PRIu32, (uint32_t)(entity & ECS_ENTITY_MASK));
    cJSON_AddStringToObject(message, "entity_id", buffer);

    // only 16 bits. won't overflow buffer
    snprintf(buffer, sizeof(buffer), "%" PRIu16, (uint16_t)ECS_GENERATION(entity));
    cJSON_AddStringToObject(message, "entity_generation", buffer);
}

static void log_event_id(
    cJSON *message,
    uint64_t event_id
) {
    char buffer[sizeof("18,446,744,073,709,551,615")];
    snprintf(buffer, sizeof(buffer), "%" PRIu64, event_id);
    cJSON_AddStringToObject(message, "event_id", buffer);
}

static void log_dec(
    void *allocation,
    const char *submethod,
    const char *file,
    const char *function,
    int line,
    uint64_t event_id
) {
    gc_header *header = get_header(allocation);
    cJSON *message = create_log_message("cortecs_gc_dec");
    cJSON_AddStringToObject(message, "submethod", submethod);
    log_source_location(message, file, function, line);

    log_event_id(message, event_id);

    log_type_info(
        message,
        header->type & ARRAY_BIT_CLEAR,
        (header->type & ARRAY_BIT_ON) == ARRAY_BIT_ON
    );
    log_allocation_info(message, allocation, header->entity);
    CN(Cortecs, Log, write)(log_stream, message);
    cJSON_Delete(message);
}

static void log_alloc(
    const char *method,
    const char *file,
    const char *function,
    int line,
    cortecs_finalizer_index finalizer_index,
    bool is_array,
    void *allocation,
    ecs_entity_t entity,
    int size_class
) {
    cJSON *message = create_log_message(method);
    log_source_location(message, file, function, line);
    log_type_info(message, finalizer_index, is_array);
    log_allocation_info(message, allocation, entity);

    if (size_class == CORTECS_GC_NUM_SIZES) {
        cJSON_AddStringToObject(message, "size_class", "malloc");
    } else {
        char buffer[sizeof("0xFFFF_FFFF")];
        snprintf(buffer, sizeof(buffer), "0x%" PRIu32, buffer_sizes[size_class]);
        cJSON_AddStringToObject(message, "size_class", buffer);
    }

    CN(Cortecs, Log, write)(log_stream, message);
    cJSON_Delete(message);
}

// ====================================================================================================================
// Dec Impl
// ====================================================================================================================
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
            log_event_id(message, event_id);
        }

        log_type_info(
            message,
            header->type & ARRAY_BIT_CLEAR,
            (header->type & ARRAY_BIT_ON) == ARRAY_BIT_ON
        );
        log_allocation_info(message, allocation, header->entity);
        CN(Cortecs, Log, write)(log_stream, message);
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

static void enqueue_dec(
    void *allocation,
    const char *file,
    const char *function,
    int line
) {
    ecs_entity_t entity = get_entity(allocation);
    uint64_t event_id = dec_event_id;
    dec_event_id++;

    if (log_stream != NULL) {
        log_dec(allocation, "enqueue_dec", file, function, line, event_id);
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
    if (allocation == NULL) {
        return;
    }

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

// ====================================================================================================================
// Inc Impl
// ====================================================================================================================
void cortecs_gc_inc_impl(
    void *allocation,
    const char *file,
    const char *function,
    int line
) {
    if (allocation == NULL) {
        return;
    }

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
        CN(Cortecs, Log, write)(log_stream, message);
        cJSON_Delete(message);
    }

    // Immediate increment
    // TODO make atomic
    header->count++;
}

// ====================================================================================================================
// Alloc Impl
// ====================================================================================================================
static int get_size_class(uint32_t size_of_allocation) {
    for (int size_class_index = 0; size_class_index < CORTECS_GC_NUM_SIZES; size_class_index++) {
        if (size_of_allocation < buffer_sizes[size_class_index]) {
            return size_class_index;
        }
    }
    return CORTECS_GC_NUM_SIZES;
}

static void *alloc(
    uint32_t size_of_allocation,
    cortecs_finalizer_index finalizer_index,
    uint16_t array_bit,
    const char *method,
    const char *file,
    const char *function,
    int line
) {
    ecs_entity_t entity = ecs_new(world);

    void *allocation;
    int size_class = get_size_class(size_of_allocation);
    if (size_class == CORTECS_GC_NUM_SIZES) {
        // fallback to malloc based buffer
        gc_buffer_ptr *buffer = ecs_emplace_id(world, entity, gc_buffers[CORTECS_GC_NUM_SIZES], NULL);
        allocation = malloc(sizeof(gc_header) + size_of_allocation);
        buffer->ptr = allocation;
    } else {
        allocation = ecs_emplace_id(world, entity, gc_buffers[size_class], NULL);
    }

    gc_header *header = allocation;
    header->entity = entity;
    header->type = finalizer_index | array_bit;
    header->count = 1;

    void *out_pointer = (void *)((uintptr_t)allocation + sizeof(gc_header));

    if (log_stream != NULL) {
        log_alloc(
            method,
            file,
            function,
            line,
            finalizer_index,
            array_bit == ARRAY_BIT_ON,
            out_pointer,
            entity,
            size_class
        );
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

// ====================================================================================================================
// Init/cleanup impl
// ====================================================================================================================
void cortecs_gc_init_impl(
    // log_path needs to be const char * because it's impossible
    // to construct a cortecs_string before GC is initialized
    const char *log_path,
    const char *file,
    const char *function,
    int line
) {
    // initialize the various size classes
    char name[NAME_MAX_SIZE];
    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        uint32_t size = buffer_sizes[i];
        snprintf(name, NAME_MAX_SIZE, "gc_buffer_%" PRIu32, size);
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
        ecs_defer_begin(world);

        // allocate the log stream
        // the log stream is required to log the allocation messages.
        // these drop the alloc and dec messages created during memory allocation.
        // we spoof these messages after the init message for log completeness
        uint64_t string_event_id = dec_event_id;
        CN(Cortecs, String) log_path_string = CN(Cortecs, String, new)("%s", log_path);
        uint64_t log_stream_event_id = dec_event_id;
        log_stream = CN(Cortecs, Log, open)(log_path_string);

        // log init message
        cJSON *message = create_log_message("cortecs_gc_init");
        log_source_location(message, file, function, line);
        cJSON_AddStringToObject(message, "log_path", log_path);
        CN(Cortecs, Log, write)(log_stream, message);
        cJSON_Delete(message);

        // spoof log_path_string log messages
        log_alloc(
            "cortecs_gc_alloc",
            file,
            function,
            line,
            cortecs_finalizer_index_name(CN(Cortecs, Char)), false, log_path_string.content, get_entity(log_path_string.content), get_size_class(sizeof(CN(Cortecs, Char))));
        log_dec(log_path_string.content, "enqueue_dec", file, function, line, string_event_id);

        // spoof log_stream log messages
        log_alloc(
            "cortecs_gc_alloc",
            file,
            function,
            line,
            cortecs_finalizer_index_name(CN(Cortecs, Log)), false, log_stream, get_entity(log_stream), get_size_class(sizeof(struct CN(Cortecs, Log))));
        log_dec(log_stream, "enqueue_dec", file, function, line, log_stream_event_id);

        // keep the log stream but not the string
        cortecs_gc_inc_impl(log_stream, file, function, line);

        ecs_defer_end(world);
    } else {
        log_stream = NULL;
    }
}

void cortecs_gc_cleanup_impl(
    const char *file,
    const char *function,
    int line
) {
    if (log_stream) {
        // the dec cleans up the log stream causing a use-after-free error if
        // the cleanup message is logged after the dec, but we want the dec
        // message to come before the cleanup message in the logs.

        // Spoof the dec message so that it comes before the cleanup message in the logs
        log_dec(log_stream, "perform_dec", file, function, line, 0);

        cJSON *message = create_log_message("cortecs_gc_cleanup");
        log_source_location(message, file, function, line);
        CN(Cortecs, Log, write)(log_stream, message);
        cJSON_Delete(message);

        // null out the global log_stream so that the dec api
        // call doesnt log a message
        CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) log_stream_local = log_stream;
        log_stream = NULL;

        // make sure this comes last too avoid use-after-free
        cortecs_gc_dec(log_stream_local);
    }
}

bool cortecs_gc_is_alive(void *allocation) {
    // TODO this api should be removed in favor of using logs
    gc_header header = *(gc_header *)allocation;
    UNUSED(header);

    ecs_entity_t entity = get_entity(allocation);
    return ecs_is_alive(world, entity);
}
