#ifndef CORTECS_GC_GC_H
#define CORTECS_GC_GC_H

#include <stdint.h>

void *cortecs_gc_alloc(uint32_t size);
void cortecs_gc_init();
void cortecs_gc_inc(void *allocation);
void cortecs_gc_dec(void *allocation);

#endif