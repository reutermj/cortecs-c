#ifndef CORTECS_STRING_STRING_H
#define CORTECS_STRING_STRING_H

#include <cortecs/finalizer.h>
#include <cortecs/types.h>
#include <inttypes.h>
#include <stdbool.h>

// Strings are encoded using utf-8 to support unicode and
// maintain compatibility with C/OS api that expect ascii encoding.

typedef struct CN(Cortecs, String) {
    CN(Cortecs, Array, CT(CN(Cortecs, Char))) content;
} CN(Cortecs, String);

CN(Cortecs, String) CN(Cortecs, String, new)(const char *format, ...);
uint32_t CN(Cortecs, String, capacity)(CN(Cortecs, String) str);
bool CN(Cortecs, String, equals)(CN(Cortecs, String) left, CN(Cortecs, String) right);

#endif