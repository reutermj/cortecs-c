#ifndef COMMON_COMMON_H
#define COMMON_COMMON_H

#define UNUSED(X) (void)(X)

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a##b

#define NOOP                         \
    int CONCAT(NOOP_, __LINE__) = 0; \
    UNUSED(CONCAT(NOOP_, __LINE__));

#define MASK(BITS) (~(UINT64_MAX << BITS))

#define COMPILE_TIME_ASSERT(predicate) \
    static char CONCAT(compile_time_assertion_, line)[!!(predicate)-1];

#endif