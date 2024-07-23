#include <assert.h>
#include <common.h>
#include <flecs.h>
#include <gc.h>
#include <stdio.h>
#include <stdlib.h>
#include <world.h>

#define CORTECS_GC_NUM_SIZES 5
static const uint32_t buffer_sizes[CORTECS_GC_NUM_SIZES] = {32, 64, 128, 256, 512};
const uint32_t name_max_size = sizeof("gc_buffer_512");
static ecs_entity_t gc_buffers[CORTECS_GC_NUM_SIZES + 1];

typedef struct {
    uint32_t count;
} reference_count;
static ECS_COMPONENT_DECLARE(reference_count);

typedef struct {
    void *ptr;
} gc_buffer_ptr;

static void on_inc(ecs_iter_t *iterator) {
    reference_count *count = ecs_field(iterator, reference_count, 0);

    for (int i = 0; i < iterator->count; i++) {
        printf("incing\n");
        count->count++;
    }
}

static void on_dec(ecs_iter_t *iterator) {
    reference_count *count = ecs_field(iterator, reference_count, 0);

    for (int i = 0; i < iterator->count; i++) {
        printf("decing\n");
        count->count--;
        if (count->count == 0) {
            printf("deleting\n");
            ecs_delete(world, iterator->entities[i]);
        }
    }
}

static void free_gc_buffer_ptr(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
    UNUSED(type_info);
    gc_buffer_ptr *buffers = (gc_buffer_ptr *)ptr;
    for (int i = 0; i < count; i++) {
        free(buffers[i].ptr);
    }
}

static ecs_entity_t inc;
static ecs_entity_t dec;

void cortecs_gc_init() {
    ECS_COMPONENT_DEFINE(world, reference_count);

    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        uint32_t size = buffer_sizes[i];
        char name[name_max_size];
        snprintf(name, name_max_size, "gc_buffer_%d", size);
        ecs_component_desc_t desc = {
            .entity = ecs_entity_init(world, &(ecs_entity_desc_t){.name = name}),
            .type = {
                .size = ECS_SIZEOF(ecs_entity_t) + size,
                .alignment = ECS_ALIGNOF(ecs_entity_t) + size,
            },
        };
        gc_buffers[i] = ecs_component_init(world, &desc);
        // marking the buffer as sparse makes sure it doesnt get moved and
        // pointers to it are stable
        ecs_add_id(world, gc_buffers[i], EcsSparse);
    }

    ecs_component_desc_t desc = {
        .entity = ecs_entity_init(world, &(ecs_entity_desc_t){.name = "gc_buffer_ptr"}),
        .type = {
            .size = ECS_SIZEOF(gc_buffer_ptr),
            .alignment = ECS_ALIGNOF(gc_buffer_ptr),
            .hooks = {
                .dtor = free_gc_buffer_ptr,
            },
        },
    };
    gc_buffers[CORTECS_GC_NUM_SIZES] = ecs_component_init(world, &desc);
    // marking the buffer as sparse makes sure it doesnt get moved and
    // pointers to it are stable
    ecs_add_id(world, gc_buffers[CORTECS_GC_NUM_SIZES], EcsSparse);

    inc = ecs_new(world);
    ecs_observer_desc_t inc_desc = {
        .query = {
            .terms[0].id = ecs_id(reference_count),
        },
        .events[0] = inc,
        .callback = on_inc,
    };
    ecs_observer_init(world, &inc_desc);

    dec = ecs_new(world);
    ecs_observer_desc_t dec_desc = {
        .query = {
            .terms[0].id = ecs_id(reference_count),
        },
        .events[0] = dec,
        .callback = on_dec,
    };
    ecs_observer_init(world, &dec_desc);
}

void gc_dec(ecs_entity_t target) {
    ecs_event_desc_t dec_event = {
        .event = dec,
        .entity = target,
        .ids = &(ecs_type_t){
            .array = ecs_ids(ecs_id(reference_count)),
            .count = 1,
        },
    };
    ecs_enqueue(world, &dec_event);
}

static void *alloc(uint32_t size, ecs_entity_t entity) {
    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        if (size < buffer_sizes[i]) {
            ecs_entity_t *memory = ecs_emplace_id(world, entity, gc_buffers[i], NULL);
            memory[0] = entity;
            return &memory[1];
        }
    }

    gc_buffer_ptr *buffer = ecs_emplace_id(world, entity, gc_buffers[CORTECS_GC_NUM_SIZES], NULL);
    ecs_entity_t *memory = malloc(sizeof(ecs_entity_t) + size);
    buffer->ptr = memory;
    memory[0] = entity;
    return &memory[1];
}

void *cortecs_gc_alloc(uint32_t size) {
    ecs_entity_t entity = ecs_new(world);
    int buffer_index = 0;
    for (; buffer_index < CORTECS_GC_NUM_SIZES; buffer_index++) {
        if (size < buffer_sizes[buffer_index]) {
            break;
        }
    }

    void *memory = alloc(size, entity);
    ecs_set(world, entity, reference_count, {.count = 1});
    gc_dec(entity);

    return memory;
}

ecs_entity_t get_allocation_entity(void *allocation) {
    ecs_entity_t *entity = (ecs_entity_t *)(((uintptr_t)allocation) - sizeof(ecs_entity_t));
    return *entity;
}

void cortecs_gc_inc(void *allocation) {
    ecs_event_desc_t inc_event = {
        .event = inc,
        .entity = get_allocation_entity(allocation),
        .ids = &(ecs_type_t){
            .array = ecs_ids(ecs_id(reference_count)),
            .count = 1,
        },
    };
    ecs_emit(world, &inc_event);
}

void cortecs_gc_dec(void *allocation) {
    gc_dec(get_allocation_entity(allocation));
}
