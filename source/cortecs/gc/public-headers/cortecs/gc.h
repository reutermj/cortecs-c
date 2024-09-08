#ifndef CORTECS_GC_GC_H
#define CORTECS_GC_GC_H

#include <common.h>
#include <stdbool.h>
#include <stdint.h>

#define CORTECS_GC_NO_FINALIZER 0
#define CORTECS_GC_ALLOCATION 1

typedef void (*cortecs_gc_finalizer_type)(void *allocation);
typedef uint16_t cortecs_gc_finalizer_index;

cortecs_gc_finalizer_index cortecs_gc_register_finalizer(cortecs_gc_finalizer_type finalizer, uintptr_t size, uintptr_t offset_of_elements);

#define cortecs_gc_finalizer(TYPE) \
    CONCAT(cortecs_gc_finalizer_, TYPE)

#define cortecs_gc_finalizer_index_name(TYPE) \
    CONCAT(cortecs_gc_finalizer(TYPE), _index)

#define cortecs_gc_finalizer_declare(TYPE) \
    cortecs_gc_finalizer_index cortecs_gc_finalizer_index_name(TYPE);

#define cortecs_gc_finalizer_init(TYPE) \
    cortecs_gc_finalizer_index_name(TYPE) = cortecs_gc_register_finalizer(cortecs_gc_finalizer(TYPE), sizeof(TYPE), offsetof(cortecs_array_name(TYPE), elements));

void *cortecs_gc_alloc_impl(uint32_t size_of_type, cortecs_gc_finalizer_index finalizer_index);
#define cortecs_gc_alloc(TYPE) \
    cortecs_gc_alloc_impl(sizeof(TYPE), cortecs_gc_finalizer_index_name(TYPE))

void *cortecs_gc_alloc_array_impl(uint32_t size_of_type, uint32_t size_of_array, uint32_t offset_of_elements, cortecs_gc_finalizer_index finalizer_index);
#define cortecs_gc_alloc_array(TYPE, SIZE) \
    cortecs_gc_alloc_array_impl(sizeof(TYPE), SIZE, offsetof(cortecs_array_name(TYPE), elements), cortecs_gc_finalizer_index_name(TYPE))

void cortecs_gc_init();
void cortecs_gc_inc(void *allocation);
void cortecs_gc_dec(void *allocation);
bool cortecs_gc_is_alive(void *allocation);

#endif