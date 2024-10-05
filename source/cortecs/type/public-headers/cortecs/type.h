#ifndef CORTECS_TYPE_TYPE_H
#define CORTECS_TYPE_TYPE_H

#include <common.h>
#include <stdalign.h>
#include <stddef.h>
#include <stdint.h>

// ====================================================================================================================
// Array
// ====================================================================================================================

#define CORTECS_ARRAY_ELEMENTS_OFFSET alignof(max_align_t)
#define cortecs_array_name(TYPE) \
    CONCAT(cortecs_array_, TYPE)

#define cortecs_array_forward_declare(TYPE) \
    typedef struct cortecs_array_name(TYPE) cortecs_array_name(TYPE)

#define cortecs_array_declare(TYPE)                                              \
    struct cortecs_array_name(TYPE) {                                            \
        uint32_t size;                                                           \
        uint32_t stride;                                                         \
        uint8_t padding[CORTECS_ARRAY_ELEMENTS_OFFSET - (2 * sizeof(uint32_t))]; \
        TYPE elements[];                                                         \
    }

#define cortecs_array(TYPE) CONCAT(cortecs_array_, TYPE) *
#define cortecs_array_generic(TYPE) cortecs_array_generic_impl *
typedef struct {
    uint32_t size;
    uint32_t stride;
    uint8_t padding[CORTECS_ARRAY_ELEMENTS_OFFSET - (2 * sizeof(uint32_t))];
    uint8_t data[];
} cortecs_array_generic_impl;

void *cortecs_array_generic_get(cortecs_array_generic(T) array, uint32_t index);
void cortecs_array_generic_set(cortecs_array_generic(T) array, uint32_t index, void *element);

cortecs_array_forward_declare(int8_t);
cortecs_array_declare(int8_t);
cortecs_array_forward_declare(int16_t);
cortecs_array_declare(int16_t);
cortecs_array_forward_declare(int32_t);
cortecs_array_declare(int32_t);
cortecs_array_forward_declare(int64_t);
cortecs_array_declare(int64_t);
cortecs_array_forward_declare(uint8_t);
cortecs_array_declare(uint8_t);
cortecs_array_forward_declare(uint16_t);
cortecs_array_declare(uint16_t);
cortecs_array_forward_declare(uint32_t);
cortecs_array_declare(uint32_t);
cortecs_array_forward_declare(uint64_t);
cortecs_array_declare(uint64_t);

typedef struct {
    uint32_t size;
} cortecs_array_void;

// ====================================================================================================================
// Type
// ====================================================================================================================

// TODO: need to express that this is not a full 16 bit type.
// 15 bits are used to express the index and 1 bit is used to for the array bit
typedef uint16_t cortecs_type_finalizer_index;

// =============================================
// || 00 01 .. 31 | 32 43 .. 46 | 47 50 .. 63 ||
// ||    size     |    index    |   unused    ||
// =============================================
typedef uint64_t cortecs_type;

#define CORTECS_TYPE_SIZE_SHIFT 0
#define CORTECS_TYPE_SIZE_NUM_BITS 32
#define CORTECS_TYPE_SIZE_MAX_VALUE MAX_VALUE(CORTECS_TYPE_SIZE_NUM_BITS)
#define CORTECS_TYPE_SIZE_MASK MASK(CORTECS_TYPE_SIZE_NUM_BITS, CORTECS_TYPE_SIZE_SHIFT)
#define CORTECS_TYPE_SIZE_CLEAR (~CORTECS_TYPE_SIZE_MASK)

#define CORTECS_TYPE_INDEX_SHIFT 32
#define CORTECS_TYPE_INDEX_NUM_BITS 15
#define CORTECS_TYPE_INDEX_MAX_VALUE MAX_VALUE(CORTECS_TYPE_INDEX_NUM_BITS)
#define CORTECS_TYPE_INDEX_MASK MASK(CORTECS_TYPE_INDEX_NUM_BITS, CORTECS_TYPE_INDEX_SHIFT)
#define CORTECS_TYPE_INDEX_CLEAR (~CORTECS_TYPE_INDEX_MASK)

uint64_t cortecs_type_size(cortecs_type type);
cortecs_type_finalizer_index cortecs_type_index(cortecs_type type);

#define cortecs_type_forward_declare(TYPE) \
    cortecs_array_forward_declare(TYPE);   \
    extern cortecs_type cortecs_type_declaration_name(TYPE)

#define cortecs_type_declare(TYPE) \
    cortecs_array_declare(TYPE)

#define cortecs_type_define(TYPE) \
    cortecs_type cortecs_type_declaration_name(TYPE) = (sizeof(TYPE) << CORTECS_TYPE_SIZE_SHIFT)

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