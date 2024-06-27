#ifndef COMMON_PERSISTENT_STRING_H
#define COMMON_PERSISTENT_STRING_H

#include <inttypes.h>
#include <stdbool.h>

typedef struct {
    uint32_t length;
    uint8_t *content;
} string_t;

bool string_equals(string_t left, string_t right);
string_t string_copy_cstring(const char *target);
void string_cleanup(string_t target);

#endif