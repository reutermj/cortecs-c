#ifndef CORTECS_STDLIB_HASHMAP_HASHMAP_H
#define CORTECS_STDLIB_HASHMAP_HASHMAP_H

#include <common.h>

#define cortecs_hashmap(KEY, VALUE) CONCAT(cortecs_hashmap_, CONCAT(KEY, CONCAT(_, VALUE)))
#define cortecs_hashmap_branch(KEY, VALUE) CONCAT(cortecs_hashmap(KEY, VALUE), _branch)
#define cortecs_hashmap_bucket(KEY, VALUE) CONCAT(cortecs_hashmap(KEY, VALUE), _bucket)
#define cortecs_hashmap_new(KEY, VALUE) CONCAT(cortecs_hashmap(KEY, VALUE), _new)
#define cortecs_hashmap_set(KEY, VALUE) CONCAT(cortecs_hashmap(KEY, VALUE), _set)
#define cortecs_hashmap_get(KEY, VALUE) CONCAT(cortecs_hashmap(KEY, VALUE), _get)
#define cortecs_hashmap_register_finalizer(KEY, VALUE) CONCAT(cortecs_hashmap(KEY, VALUE), _register_finalizer)

typedef enum {
    CORTECS_HASHMAP_NONE,
    CORTECS_HASHMAP_BRANCH,
    CORTECS_HASHMAP_BUCKET,
} cortecs_hashmap_tag;

#endif