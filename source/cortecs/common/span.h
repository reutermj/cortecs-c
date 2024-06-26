#ifndef CORTECS_COMMON_SPAN_H
#define CORTECS_COMMON_SPAN_H

#include <flecs.h>
#include <persistent_string.h>
#include <stdint.h>

typedef struct {
    uint32_t lines;
    uint32_t columns;
} cortecs_span_t;
extern ECS_COMPONENT_DECLARE(cortecs_span_t);

int cortecs_span_compare(cortecs_span_t left, cortecs_span_t right);
cortecs_span_t cortecs_span_of(string_t text);
cortecs_span_t cortecs_span_add(cortecs_span_t left, cortecs_span_t right);

#endif