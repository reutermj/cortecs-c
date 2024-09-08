#ifndef CORTECS_STRING_STRING_H
#define CORTECS_STRING_STRING_H

#include <cortecs/array.h>
#include <cortecs/gc.h>
#include <inttypes.h>
#include <stdbool.h>

// Strings are encoded using utf-8 to support unicode and
// maintain compatibility with C/OS api that expect ascii encoding.

typedef struct {
    uint32_t size;
    char content[];
} cortecs_string_impl;
typedef cortecs_string_impl *cortecs_string;
extern cortecs_gc_finalizer_declare(cortecs_string);
cortecs_array_declare(cortecs_string);

cortecs_string cortecs_string_new(const char *format, ...);
bool cortecs_string_equals(cortecs_string left, cortecs_string right);

#endif