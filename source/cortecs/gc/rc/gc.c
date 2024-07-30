#include <assert.h>
#include <common.h>
#include <flecs.h>
#include <gc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <world.h>

#define CORTECS_GC_NUM_SIZES 5
static const uint32_t buffer_sizes[CORTECS_GC_NUM_SIZES] = {32, 64, 128, 256, 512};
static const uint32_t name_max_size = sizeof("gc_buffer_512");
static ecs_entity_t gc_buffers[CORTECS_GC_NUM_SIZES + 1];

typedef struct {
    void *ptr;
} gc_buffer_ptr;

static ecs_entity_t get_allocation_entity(void *allocation) {
    ecs_entity_t *entity = (ecs_entity_t *)((uintptr_t)allocation - sizeof(uint32_t) - sizeof(ecs_entity_t));
    return *entity;
}

static uint32_t *get_reference_count(void *allocation) {
    return (uint32_t *)((uintptr_t)allocation - sizeof(uint32_t));
}

typedef struct {
    void *allocation;
} dec;
static ECS_COMPONENT_DECLARE(dec);

static void on_dec(ecs_iter_t *iterator) {
    assert(iterator->count == 1);
    printf("decing\n");
    dec *event = iterator->param;
    uint32_t *reference_count = get_reference_count(event->allocation);
    uint32_t count = *reference_count - 1;
    if (count == 0) {
        printf("deleting\n");
        ecs_delete(world, iterator->entities[0]);
    } else {
        *reference_count = count;
    }
}

static void free_gc_buffer_ptr(void *ptr, int32_t count, const ecs_type_info_t *type_info) {
    UNUSED(type_info);
    gc_buffer_ptr *buffers = (gc_buffer_ptr *)ptr;
    for (int i = 0; i < count; i++) {
        free(buffers[i].ptr);
    }
}

void cortecs_gc_init() {
    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        uint32_t size = buffer_sizes[i];
        char name[name_max_size];
        snprintf(name, name_max_size, "gc_buffer_%d", size);
        ecs_component_desc_t desc = {
            .entity = ecs_entity_init(world, &(ecs_entity_desc_t){.name = name}),
            .type = {
                .size = ECS_SIZEOF(ecs_entity_t) + ECS_SIZEOF(uint32_t) + size,
                .alignment = ECS_ALIGNOF(ecs_entity_t) + ECS_ALIGNOF(uint32_t) + size,
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
    ecs_event_desc_t dec_event = {
        .event = ecs_id(dec),
        .entity = target,
        .param = &(dec){.allocation = allocation},
    };
    ecs_enqueue(world, &dec_event);
}

static void *alloc(uint32_t size, ecs_entity_t entity) {
    void *allocation;
    for (int i = 0; i < CORTECS_GC_NUM_SIZES; i++) {
        if (size < buffer_sizes[i]) {
            allocation = ecs_emplace_id(world, entity, gc_buffers[i], NULL);
            goto initialize_allocation;
        }
    }

    gc_buffer_ptr *buffer = ecs_emplace_id(world, entity, gc_buffers[CORTECS_GC_NUM_SIZES], NULL);
    allocation = malloc(sizeof(ecs_entity_t) + sizeof(uint32_t) + size);
    buffer->ptr = allocation;

initialize_allocation:;  // clang is complaining about expected expression if this semicolon isnt here

    ecs_entity_t *entity_location = allocation;
    *entity_location = entity;
    uint32_t *count_location = (uint32_t *)((uintptr_t)allocation + sizeof(ecs_entity_t));
    *count_location = 1;

    return (void *)((uintptr_t)count_location + sizeof(uint32_t));
}

void *cortecs_gc_alloc(uint32_t size) {
    ecs_entity_t entity = ecs_new(world);
    int buffer_index = 0;
    for (; buffer_index < CORTECS_GC_NUM_SIZES; buffer_index++) {
        if (size < buffer_sizes[buffer_index]) {
            break;
        }
    }

    void *allocation = alloc(size, entity);
    gc_dec(entity, allocation);

    return allocation;
}

void cortecs_gc_inc(void *allocation) {
    // TODO make atomic
    uint32_t *count = get_reference_count(allocation);
    *count = *count + 1;
}

void cortecs_gc_dec(void *allocation) {
    gc_dec(get_allocation_entity(allocation), allocation);
}
