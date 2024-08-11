#include <world.h>

ecs_world_t *world;

void cortecs_world_init() {
    world = ecs_init();
    ecs_set_entity_range(world, CORTECS_APPLICATION_ENTITY_MIN, CORTECS_APPLICATION_ENTITY_MAX);
}

void cortecs_world_cleanup() {
    ecs_fini(world);
}