#ifndef CORTECS_GC_GC_H
#define CORTECS_GC_GC_H

#include <stdbool.h>
#include <stdint.h>

typedef void (*cortecs_gc_finalizer)(void *allocation);
typedef uint16_t cortecs_gc_type_index;

void *cortecs_gc_alloc(uint32_t size, cortecs_gc_type_index type_index);
void *cortecs_gc_alloc_array(uint32_t size, uint32_t elements, cortecs_gc_type_index type);
cortecs_gc_type_index cortecs_gc_register_type(cortecs_gc_finalizer finalizer, uint32_t size);
void cortecs_gc_init();
void cortecs_gc_inc(void *allocation);
void cortecs_gc_dec(void *allocation);
bool cortecs_gc_is_alive(void *allocation);

#endif