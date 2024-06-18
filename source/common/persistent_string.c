#include <persistent_string.h>
#include <stdlib.h>
#include <string.h>

bool string_equals(string_t left, string_t right) {
    if (left.length != right.length) {
        return false;
    }

    return strncmp((const char *)left.content, (const char *)right.content, left.length) == 0;
}

void string_cleanup(string_t target) {
    if (target.content != NULL) {
        free(target.content);
    }
}