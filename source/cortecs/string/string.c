#include <cortecs/string.h>
#include <stdlib.h>
#include <string.h>

string_t string_copy_cstring(const char *target) {
    uint32_t length = strlen(target);
    uint8_t *content = malloc(length + 1);
    memcpy(content, target, length);
    content[length] = 0;
    return (string_t){
        .length = length,
        .content = content,
    };
}

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