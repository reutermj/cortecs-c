#include <cortecs/world.h>

ecs_world_t *world;

void cortecs_world_init() {
    world = ecs_init();
}

void cortecs_world_cleanup() {
    ecs_fini(world);
}