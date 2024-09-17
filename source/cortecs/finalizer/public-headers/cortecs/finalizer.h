#ifndef CORTECS_FINALIZER_FINALIZER_H
#define CORTECS_FINALIZER_FINALIZER_H

#include <common.h>
#include <stdint.h>

#define CORTECS_FINALIZER_NONE 0

typedef void (*cortecs_finalizer_type)(void *allocation);
typedef uint16_t cortecs_finalizer_index;

#define cortecs_finalizer(TYPE) \
    CONCAT(cortecs_finalizer_, TYPE)

#define cortecs_finalizer_index_name(TYPE) \
    CONCAT(cortecs_finalizer(TYPE), _index)

#define cortecs_finalizer_declare(TYPE) \
    cortecs_finalizer_index cortecs_finalizer_index_name(TYPE);

#define cortecs_finalizer_define(TYPE) \
    cortecs_finalizer_index cortecs_finalizer_index_name(TYPE) = CORTECS_FINALIZER_NONE;

#define cortecs_finalizer_register(TYPE)                                        \
    cortecs_finalizer_index_name(TYPE) = cortecs_finalizer_register_impl(       \
        (cortecs_finalizer_metadata){                                           \
            .type_name = #TYPE,                                                 \
            .finalizer = cortecs_finalizer(TYPE),                               \
            .size = sizeof(TYPE),                                               \
            .offset_of_elements = offsetof(cortecs_array_name(TYPE), elements), \
        }                                                                       \
    );

typedef struct {
    const char *type_name;
    cortecs_finalizer_type finalizer;
    uintptr_t size;
    uintptr_t offset_of_elements;
} cortecs_finalizer_metadata;

void cortecs_finalizer_init();
cortecs_finalizer_index cortecs_finalizer_register_impl(cortecs_finalizer_metadata metadata);
cortecs_finalizer_metadata cortecs_finalizer_get(cortecs_finalizer_index index);

#endif