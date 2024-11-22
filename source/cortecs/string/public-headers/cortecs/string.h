#ifndef CORTECS_STRING_STRING_H
#define CORTECS_STRING_STRING_H

#include <cortecs/finalizer.h>
#include <cortecs/mangle.h>
#include <inttypes.h>
#include <stdbool.h>

// Strings are encoded using utf-8 to support unicode and
// maintain compatibility with C/OS api that expect ascii encoding.

struct CN(Cortecs, String) {
    uint32_t size;
    char content[];
};

typedef struct CN(Cortecs, String) * CN(Cortecs, String);
extern cortecs_finalizer_declare(CN(Cortecs, String));

CN(Cortecs, String) CN(Cortecs, String, new)(const char *format, ...);
bool CN(Cortecs, String, equals)(CN(Cortecs, String) left, CN(Cortecs, String) right);

#endif