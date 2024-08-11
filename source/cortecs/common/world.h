#ifndef CORTECS_COMMON_WORLD_H
#define CORTECS_COMMON_WORLD_H

#include <flecs.h>

#define CORTECS_APPLICATION_ENTITY_MIN 0
#define CORTECS_APPLICATION_ENTITY_MAX 0xFFF00000

extern ecs_world_t *world;

void cortecs_world_init();
void cortecs_world_cleanup();

#endif