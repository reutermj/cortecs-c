#include <assert.h>
#include <flecs.h>
#include <gc.h>
#include <world.h>

// currently only allowing buffer size of 256. Will expand later
#define CORTECS_GC_ALLOC_MAX_SIZE 256
typedef struct {
    uint32_t count;
    ecs_entity_t entity;
    uint8_t data[CORTECS_GC_ALLOC_MAX_SIZE];
} gc_buffer;
static ECS_COMPONENT_DECLARE(gc_buffer);

static void on_inc(ecs_iter_t *iterator) {
    gc_buffer *count = ecs_field(iterator, gc_buffer, 0);
    for (int i = 0; i < iterator->count; i++) {
        printf("incing\n");
        count->count++;
    }
}

static void on_dec(ecs_iter_t *iterator) {
    gc_buffer *count = ecs_field(iterator, gc_buffer, 0);
    int32_t target_index = ecs_query_find_var(iterator->query, "Target");

    for (int i = 0; i < iterator->count; i++) {
        printf("decing\n");
        count->count--;
        if (count->count == 0) {
            printf("deleting\n");
            ecs_entity_t target = ecs_iter_get_var(iterator, target_index);
            ecs_delete(world, target);
        }
    }
}

static ecs_entity_t inc;
static ecs_entity_t dec;

void cortecs_gc_init() {
    ECS_COMPONENT_DEFINE(world, gc_buffer);
    // marking the buffer as sparse makes sure it doesnt get moved and
    // pointers to it are stable
    ecs_add_id(world, ecs_id(gc_buffer), EcsSparse);

    inc = ecs_new(world);
    ecs_entity_desc_t inc_reference_count_entity = {
        .name = "inc_reference_count_system",
        .add = ecs_ids(ecs_dependson(EcsPostFrame)),
    };
    ecs_system_desc_t inc_reference_count_system = {
        .entity = ecs_entity_init(world, &inc_reference_count_entity),
        .query = {
            .terms[0] = {
                .src.name = "$Target",
                .first.id = ecs_id(gc_buffer),
            },
            .terms[1] = {
                .src.id = EcsThis,
                .first.id = inc,
                .second.name = "$Target",
            },
        },
        .callback = on_inc,
    };
    ecs_system_init(world, &inc_reference_count_system);

    dec = ecs_new(world);
    ecs_entity_desc_t dec_reference_count_entity = {
        .name = "dec_reference_count_system",
        .add = ecs_ids(ecs_dependson(EcsPostFrame)),
    };
    ecs_system_desc_t dec_reference_count_system = {
        .entity = ecs_entity_init(world, &dec_reference_count_entity),
        .query = {
            .terms[0] = {
                .src.name = "$Target",
                .first.id = ecs_id(gc_buffer),
            },
            .terms[1] = {
                .src.id = EcsThis,
                .first.id = dec,
                .second.name = "$Target",
            },
        },
        .callback = on_dec,
    };
    ecs_system_init(world, &dec_reference_count_system);
}

void gc_dec(ecs_entity_t target) {
    ecs_entity_t source = ecs_new(world);
    ecs_add_pair(world, source, dec, target);
}

void *cortecs_gc_alloc(uint32_t size) {
    assert(size <= CORTECS_GC_ALLOC_MAX_SIZE);

    ecs_entity_t entity = ecs_new(world);
    gc_buffer *memory = ecs_emplace_id(world, entity, ecs_id(gc_buffer), NULL);
    memory->entity = entity;
    memory->count = 1;
    gc_dec(entity);

    return memory->data;
}

ecs_entity_t get_allocation_entity(void *allocation) {
    gc_buffer *buffer = (gc_buffer *)(((uintptr_t)allocation) - offsetof(gc_buffer, data));
    return buffer->entity;
}

void cortecs_gc_inc(void *allocation) {
    ecs_entity_t target = get_allocation_entity(allocation);
    ecs_entity_t source = ecs_new(world);
    ecs_add_pair(world, source, inc, target);
}

void cortecs_gc_dec(void *allocation) {
    gc_dec(get_allocation_entity(allocation));
}
