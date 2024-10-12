#include <cortecs/array.h>

#include "hashmap.h"

#ifndef TYPE_PARAM_KEY
#error "Expected TYPE_PARAM_KEY to be define"
#endif

#ifndef TYPE_PARAM_VALUE
#error "Expected TYPE_PARAM_VALUE to be defined"
#endif

typedef struct cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) * cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE);
cortecs_array_forward_declare(cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE));
cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) cortecs_hashmap_new(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)();
cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) cortecs_hashmap_set(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)(
    cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) map,
    TYPE_PARAM_KEY key,
    TYPE_PARAM_VALUE value
);
TYPE_PARAM_VALUE cortecs_hashmap_get(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)(
    cortecs_hashmap(TYPE_PARAM_KEY, TYPE_PARAM_VALUE) map,
    TYPE_PARAM_KEY key
);
void cortecs_hashmap_register_finalizer(TYPE_PARAM_KEY, TYPE_PARAM_VALUE)();