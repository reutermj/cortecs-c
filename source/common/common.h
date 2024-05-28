#ifndef COMMON_H
#define COMMON_H

#define UNUSED(X) (void)(X)

#define CONCAT(a, b) CONCAT_INNER(a, b)
#define CONCAT_INNER(a, b) a##b

#define NOOP                         \
    int CONCAT(NOOP_, __LINE__) = 0; \
    UNUSED(CONCAT(NOOP_, __LINE__));

#endif