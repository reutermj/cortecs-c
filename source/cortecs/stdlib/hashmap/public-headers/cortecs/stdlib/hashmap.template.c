#include "hashmap.h"

#include <cortecs/array.h>
#include <cortecs/finalizer.h>
#include <cortecs/gc.h>
#include <stdlib.h>

#ifndef TYPE_PARAM_KEY
#error "Expected TYPE_PARAM_KEY to be defined"
#endif

#ifndef TYPE_PARAM_VALUE
#error "Expected TYPE_PARAM_VALUE to be defined"
#endif

typedef struct {
    cortecs_array(TYPE_PARAM_KEY) keys;
    cortecs_array(TYPE_PARAM_VALUE) values;
} cortecs_hashmap_bucket(TYPE_PARAM_KEY, TYPE_PARAM_VALUE);

typedef struct {
    cortecs_array(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)) branches;
} cortecs_hashmap_branch(TYPE_PARAM_KEY, TYPE_PARAM_VALUE);

struct cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) {
    cortecs_hashmap_tag tag;
    union {
        cortecs_hashmap_bucket(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) bucket;
        cortecs_hashmap_branch(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) branch;
    } value;
};
cortecs_array_define(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE));
cortecs_finalizer_define(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE));

cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) cortecs_hashmap_new(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)() {
    cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) map = cortecs_gc_alloc(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE));
    map->tag = CORTECS_HASHMAP_NONE;
    return map;
}