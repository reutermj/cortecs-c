#ifndef CORTECS_GC_GC_H
#define CORTECS_GC_GC_H

#include <cortecs/finalizer.h>
#include <stdbool.h>
#include <stdint.h>

void cortecs_gc_init_impl(
    const char *log_path,
    const char *file,
    const char *function,
    int line
);
#define cortecs_gc_init(LOG_PATH) \
    cortecs_gc_init_impl(         \
        LOG_PATH,                 \
        __FILE__,                 \
        __func__,                 \
        __LINE__                  \
    )

void *cortecs_gc_alloc_impl(
    uint32_t size_of_type,
    cortecs_finalizer_index finalizer_index,
    const char *file,
    const char *function,
    int line
);
#define cortecs_gc_alloc(TYPE)              \
    cortecs_gc_alloc_impl(                  \
        sizeof(TYPE),                       \
        cortecs_finalizer_index_name(TYPE), \
        __FILE__,                           \
        __func__,                           \
        __LINE__                            \
    )

void *cortecs_gc_alloc_array_impl(
    uint32_t size_of_type,
    uint32_t size_of_array,
    uint32_t offset_of_elements,
    cortecs_finalizer_index finalizer_index,
    const char *file,
    const char *function,
    int line
);
#define cortecs_gc_alloc_array(TYPE, SIZE)            \
    cortecs_gc_alloc_array_impl(                      \
        sizeof(TYPE),                                 \
        SIZE,                                         \
        offsetof(cortecs_array_name(TYPE), elements), \
        cortecs_finalizer_index_name(TYPE),           \
        __FILE__,                                     \
        __func__,                                     \
        __LINE__                                      \
    )

void cortecs_gc_inc_impl(
    void *allocation,
    const char *file,
    const char *function,
    int line
);
#define cortecs_gc_inc(allocation) \
    cortecs_gc_inc_impl(           \
        allocation,                \
        __FILE__,                  \
        __func__,                  \
        __LINE__                   \
    )

void cortecs_gc_dec_impl(
    void *allocation,
    const char *file,
    const char *function,
    int line
);
#define cortecs_gc_dec(allocation) \
    cortecs_gc_dec_impl(           \
        allocation,                \
        __FILE__,                  \
        __func__,                  \
        __LINE__                   \
    )

bool cortecs_gc_is_alive(void *allocation);

#endif