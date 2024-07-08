#include <assert.h>
#include <gc.h>
#include <world.h>

// currently only allowing buffer size of 256. Will expand later
#define CORTECS_GC_ALLOC_MAX_SIZE 256
typedef struct {
    uint8_t data[CORTECS_GC_ALLOC_MAX_SIZE];
} gc_buffer;
static ECS_COMPONENT_DECLARE(gc_buffer);

typedef struct {
    bool is_root_reachable;
} mark_sweep_data;
static ECS_COMPONENT_DECLARE(mark_sweep_data);

// used to signify that an allocation in directly connected to a "live" entity
static ecs_entity_t root;

// used to signify that an allocation is connected to another dynamic allocation
static ecs_entity_t reachable;

// Basic mark-sweep algorithm:
// Definitions:
// An allocation is rooted iff it is directly accessible to a component on some entity
// An allocation is reachable iff it is accessible to some other memory
// An allocation is root_reachable given the following rules:
//   1) A root_reachable B <- A root B
//   2) A root_reachable B <- A reachable C & C root_reachable B

// Steps:
// 1) mark: iterate all root_reachable memory and set the mark bit
// 2) sweep: iterate all allocations
//    2.1) delete nodes that arent root_reachable
//    2.2) reset the mark bit on nodes that are

void mark(ecs_iter_t *iterator) {
    mark_sweep_data *data = ecs_field(iterator, mark_sweep_data, 0);
    for (int i = 0; i < iterator->count; i++) {
        data[i].is_root_reachable = true;
    }
}

void sweep(ecs_iter_t *iterator) {
    mark_sweep_data *data = ecs_field(iterator, mark_sweep_data, 0);
    for (int i = 0; i < iterator->count; i++) {
        if (!data[i].is_root_reachable) {
            ecs_delete(world, iterator->entities[i]);
        } else {
            data[i].is_root_reachable = false;
        }
    }
}

void cortecs_gc_init() {
    ECS_COMPONENT_DEFINE(world, mark_sweep_data);
    ECS_COMPONENT_DEFINE(world, gc_buffer);
    // marking the buffer as sparse makes sure it doesnt get moved and
    // pointers to it are stable
    ecs_add_id(world, ecs_id(gc_buffer), EcsSparse);

    root = ecs_new(world);
    reachable = ecs_new(world);
    // the mark query traverses up reachable to find root relationships
    ecs_add_id(world, reachable, EcsTraversable);

    // define the mark system
    ecs_id_t run_on_post_frame[2] = {ecs_dependson(EcsPostFrame), 0};
    ecs_entity_desc_t mark_entity = (ecs_entity_desc_t){
        .name = "Mark",
        .add = run_on_post_frame,
    };
    ecs_system_desc_t mark_desc = (ecs_system_desc_t){
        .entity = ecs_entity_init(world, &mark_entity),
        .query = (ecs_query_desc_t){
            .terms[0].first.id = ecs_id(mark_sweep_data),
            .terms[1] = (ecs_term_t){
                // select nodes where root appears either on EcsThis or by traversing up the reachable relationship
                .src.id = EcsSelf | EcsUp,
                .first.id = root,
                .second.name = EcsWildcard,  // Dont care about the target of the root relationship
                .trav = reachable,
            },
        },
        .callback = &mark,
    };
    ecs_system_init(world, &mark_desc);

    // define the sweep system
    ECS_SYSTEM(world, sweep, EcsPostFrame, mark_sweep_data);
}

cortecs_gc_allocation_t cortecs_gc_alloc(uint32_t size) {
    assert(size <= CORTECS_GC_ALLOC_MAX_SIZE);

    ecs_entity_t entity = ecs_new(world);
    void *memory = ecs_emplace_id(world, entity, ecs_id(gc_buffer), NULL);
    ecs_set(world, entity, mark_sweep_data, {.is_root_reachable = false});

    return (cortecs_gc_allocation_t){
        .memory = memory,
        .entity = entity,
    };
}

void cortecs_gc_add(ecs_entity_t entity, cortecs_gc_allocation_t memory) {
    ecs_add_pair(world, memory.entity, reachable, entity);
}

void cortecs_gc_remove(ecs_entity_t entity, cortecs_gc_allocation_t memory) {
    ecs_remove_pair(world, memory.entity, reachable, entity);
}

void cortecs_gc_add_root(ecs_entity_t entity, cortecs_gc_allocation_t memory) {
    ecs_add_pair(world, memory.entity, root, entity);
}

void cortecs_gc_remove_root(ecs_entity_t entity, cortecs_gc_allocation_t memory) {
    ecs_remove_pair(world, memory.entity, root, entity);
}
