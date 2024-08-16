#ifndef CORTECS_WORLD_WORLD_H
#define CORTECS_WORLD_WORLD_H

#include <flecs.h>

extern ecs_world_t *world;

void cortecs_world_init();
void cortecs_world_cleanup();

#endif