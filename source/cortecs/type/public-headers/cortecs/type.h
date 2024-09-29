#ifndef CORTECS_TYPE_TYPE_H
#define CORTECS_TYPE_TYPE_H

#include <common.h>
#include <stddef.h>
#include <stdint.h>

// ====================================================================================================================
// Array
// ====================================================================================================================

#define cortecs_array_name(TYPE) \
    CONCAT(cortecs_array_, TYPE)

#define cortecs_array_forward_declare(TYPE) \
    typedef struct cortecs_array_name(TYPE) cortecs_array_name(TYPE)

#define cortecs_array_declare(TYPE)   \
    struct cortecs_array_name(TYPE) { \
        uint32_t size;                \
        TYPE elements[];              \
    }

#define cortecs_array(TYPE) CONCAT(cortecs_array_, TYPE) *

cortecs_array_declare(int8_t);
cortecs_array_declare(int16_t);
cortecs_array_declare(int32_t);
cortecs_array_declare(int64_t);
cortecs_array_declare(uint8_t);
cortecs_array_declare(uint16_t);
cortecs_array_declare(uint32_t);
cortecs_array_declare(uint64_t);

typedef struct {
    uint32_t size;
} cortecs_array_void;

// ====================================================================================================================
// Type
// ====================================================================================================================
typedef uint16_t cortecs_type_finalizer_index;

typedef struct {
    cortecs_type_finalizer_index index;
    uintptr_t size;
    uintptr_t offset_of_elements;
} cortecs_type;

#define cortecs_type_forward_declare(TYPE) \
    cortecs_array_forward_declare(TYPE);   \
    extern cortecs_type cortecs_type_declaration_name(TYPE)

#define cortecs_type_declare(TYPE) \
    cortecs_array_declare(TYPE)

#define cortecs_type_define(TYPE)                                           \
    cortecs_type cortecs_type_declaration_name(TYPE) = {                    \
        .index = CORTECS_TYPE_NO_FINALIZER,                                 \
        .size = sizeof(TYPE),                                               \
        .offset_of_elements = offsetof(cortecs_array_name(TYPE), elements), \
    }

#define cortecs_type_declaration_name(TYPE) CONCAT(cortecs_type_declaration_, TYPE)
#define cortecs_type_param(TYPE) cortecs_type cortecs_type_declaration_name(TYPE)
#define cortecs_type_arg(TYPE) cortecs_type_declaration_name(TYPE)

void cortecs_type_init();

// ====================================================================================================================
// Finalizer
// ====================================================================================================================
typedef void (*cortecs_type_finalizer_callback)(void *allocation);
typedef struct {
    const char *name;
    cortecs_type type;
    cortecs_type_finalizer_callback callback;
} cortecs_type_finalizer;

#define cortecs_finalizer(TYPE) CONCAT(cortecs_finalizer_declaration_, TYPE)
#define CORTECS_TYPE_NO_FINALIZER 0

cortecs_type cortecs_type_register_finalizer_impl(cortecs_type_param(T), cortecs_type_finalizer finalizer);
#define cortecs_type_register_finalizer(TYPE)                      \
    cortecs_type_arg(TYPE) = cortecs_type_register_finalizer_impl( \
        cortecs_type_arg(TYPE),                                    \
        (cortecs_type_finalizer){                                  \
            .name = #TYPE,                                         \
            .type = cortecs_type_arg(TYPE),                        \
            .callback = cortecs_finalizer(TYPE),                   \
        }                                                          \
    )
cortecs_type_finalizer cortecs_type_lookup_finalizer(cortecs_type_finalizer_index index);

#endif