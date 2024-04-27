#ifndef CORTECS_COMMON_COMMON_H
#define CORTECS_COMMON_COMMON_H

#include <flecs.h>
#include <stdint.h>

#define CORTECS_UNUSED(X) (void)(X)

extern ecs_world_t *cortecs_world;

typedef struct {
    uint32_t lines;
    uint32_t columns;
} cortecs_span_t;
extern ECS_COMPONENT_DECLARE(cortecs_span_t);

int cortecs_span_compare(cortecs_span_t left, cortecs_span_t right);
cortecs_span_t cortecs_span_add(cortecs_span_t left, cortecs_span_t right);

#endif