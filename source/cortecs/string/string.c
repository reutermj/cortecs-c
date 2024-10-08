#include <cortecs/gc.h>
#include <cortecs/string.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cortecs_finalizer_define(cortecs_string);

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
    cortecs_string ret = NULL;

    // measure the output string
    // there seems to be a bug in clang tidy that's false positive on this line
    int32_t size = vsnprintf(NULL, 0, format, args_size);  // NOLINT(clang-analyzer-valist.Uninitialized)
    if (size < 0) {
        // TODO encoding error. Have better error reporting
        goto cleanup;
    }

    // write the output string
    ret = cortecs_gc_alloc_impl(
        offsetof(cortecs_string_impl, content) + size + 1,
        CORTECS_FINALIZER_NONE,
        __FILE__,
        __func__,
        __LINE__
    );
    ret->size = size;
    vsnprintf(ret->content, size + 1, format, args_out);

cleanup:
    va_end(args_size);
    va_end(args_out);
    return ret;
}