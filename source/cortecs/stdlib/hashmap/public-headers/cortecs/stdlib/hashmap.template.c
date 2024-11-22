#include "hashmap.h"

#include <assert.h>
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

void cortecs_finalizer(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE))(void *allocation) {
    cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) map = allocation;
    switch (map->tag) {
        case CORTECS_HASHMAP_NONE:;
            break;
        case CORTECS_HASHMAP_BUCKET:;
            cortecs_gc_dec(map->value.bucket.keys);
            cortecs_gc_dec(map->value.bucket.values);
            break;
        case CORTECS_HASHMAP_BRANCH:;
            assert(0);
    }
}

void cortecs_hashmap_register_finalizer(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)() {
    cortecs_finalizer_register(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE));
}

cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) cortecs_hashmap_new(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)() {
    cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) map = cortecs_gc_alloc(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE));
    map->tag = CORTECS_HASHMAP_NONE;
    return map;
}

cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) cortecs_hashmap_set(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)(
    cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) map,
    TYPE_PARAM_KEY key,
    TYPE_PARAM_VALUE value
) {
    switch (map->tag) {
        case CORTECS_HASHMAP_NONE:;
            cortecs_array(TYPE_PARAM_KEY) keys = cortecs_gc_alloc_array(TYPE_PARAM_KEY, 1);
            cortecs_gc_inc(keys);
            keys->elements[0] = key;

            cortecs_array(TYPE_PARAM_VALUE) values = cortecs_gc_alloc_array(TYPE_PARAM_VALUE, 1);
            cortecs_gc_inc(values);
            values->elements[0] = value;

            cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) out = cortecs_gc_alloc(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE));
            out->tag = CORTECS_HASHMAP_BUCKET;
            out->value.bucket = (cortecs_hashmap_bucket(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)){
                .keys = keys,
                .values = values,
            };
            return out;
        case CORTECS_HASHMAP_BRANCH:;
            assert(0);  // TODO
            return NULL;
        case CORTECS_HASHMAP_BUCKET:;
            assert(0);  // TODO
            return NULL;
    }
    assert(0);  // TODO
    return NULL;
}

TYPE_PARAM_VALUE cortecs_hashmap_get(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)(
    cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) map,
    TYPE_PARAM_KEY key
) {
    switch (map->tag) {
        case CORTECS_HASHMAP_NONE:;
            assert(0);  // TODO
            return 0;
        case CORTECS_HASHMAP_BRANCH:;
            assert(0);  // TODO
            return 0;
        case CORTECS_HASHMAP_BUCKET:;
            cortecs_array(TYPE_PARAM_KEY) keys = map->value.bucket.keys;
            for (uint32_t i = 0; i < keys->size; i++) {
                if (key == keys->elements[i]) {  // todo convert to generic call
                    return map->value.bucket.values->elements[i];
                }
            }
            assert(0);
            return 0;
    }
    assert(0);
    return 0;
}