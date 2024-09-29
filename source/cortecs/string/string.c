#include <cortecs/gc.h>
#include <cortecs/string.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cortecs_type_define(cortecs_string);

bool cortecs_string_equals(cortecs_string left, cortecs_string right) {
    if (left == right) {
        return true;
    }

    if (left == NULL) {
        return false;
    }

    if (right == NULL) {
        return false;
    }

    if (left->size != right->size) {
        return false;
    }

    return strncmp(left->content, right->content, left->size) == 0;
}

cortecs_string cortecs_string_new(const char *format, ...) {
    va_list args_size;
    va_list args_out;
    va_start(args_size, format);
    va_copy(args_out, args_size);

    // measure the output string
    // there seems to be a bug in clang tidy that's false positive on this line
    int32_t size = vsnprintf(NULL, 0, format, args_size);  // NOLINT(clang-analyzer-valist.Uninitialized)
    if (size < 0) {
        // TODO encoding error. Have better error reporting
        return NULL;
    }

    // write the output string
    cortecs_string out = cortecs_gc_alloc_impl(
        (cortecs_type){
            .index = CORTECS_TYPE_NO_FINALIZER,
            .size = offsetof(struct cortecs_string, content) + size + 1,
            .offset_of_elements = 0,  // unused
        },
        __FILE__,
        __func__,
        __LINE__
    );
    out->size = size;
    vsnprintf(out->content, size + 1, format, args_out);

    va_end(args_size);
    va_end(args_out);

    return out;
}