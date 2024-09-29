#ifndef CORTECS_STRING_STRING_H
#define CORTECS_STRING_STRING_H

#include <cortecs/type.h>
#include <inttypes.h>
#include <stdbool.h>

// Strings are encoded using utf-8 to support unicode and
// maintain compatibility with C/OS api that expect ascii encoding.

struct cortecs_string {
    uint32_t size;
    char content[];
};
typedef struct cortecs_string *cortecs_string;
cortecs_type_forward_declare(cortecs_string);
cortecs_type_declare(cortecs_string);

cortecs_string cortecs_string_new(const char *format, ...);
bool cortecs_string_equals(cortecs_string left, cortecs_string right);

#endif