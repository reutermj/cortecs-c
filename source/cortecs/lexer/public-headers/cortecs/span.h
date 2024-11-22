#ifndef CORTECS_LEXER_SPAN_H
#define CORTECS_LEXER_SPAN_H

#include <cortecs/string.h>
#include <flecs.h>
#include <stdint.h>

typedef struct {
    uint32_t lines;
    uint32_t columns;
} cortecs_span_t;
extern ECS_COMPONENT_DECLARE(cortecs_span_t);

int cortecs_span_compare(cortecs_span_t left, cortecs_span_t right);
cortecs_span_t cortecs_span_of(CN(Cortecs, String) text);
cortecs_span_t cortecs_span_add(cortecs_span_t left, cortecs_span_t right);

#endif