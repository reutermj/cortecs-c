#include <assert.h>
#include <cortecs/type.h>

// Define registered type info
// reserved types:
//   0: NOOP on collection
//   1: Decrement pointer on collection (mostly useful for arrays of pointers)
#define TYPE_BITS 16
#define MAX_REGISTERED_TYPES (1 << TYPE_BITS)
static cortecs_type_finalizer registered_types[MAX_REGISTERED_TYPES];
static uint32_t next_type_index;

void cortecs_type_init() {
    next_type_index = 1;
}

cortecs_type set_index(cortecs_type_param(T), cortecs_type_finalizer_index index) {
    cortecs_type size = (cortecs_type_arg(T) & CORTECS_TYPE_INDEX_CLEAR);
    return size | ((cortecs_type)index << CORTECS_TYPE_INDEX_SHIFT);
}

cortecs_type cortecs_type_register_finalizer_impl(cortecs_type_param(T), cortecs_type_finalizer finalizer) {
    cortecs_type_param(S) = set_index(cortecs_type_arg(T), next_type_index);
    registered_types[next_type_index] = finalizer;
    next_type_index++;
    return cortecs_type_arg(S);
}

cortecs_type_finalizer cortecs_type_lookup_finalizer(cortecs_type_finalizer_index index) {
    return registered_types[index];
}

uint64_t cortecs_type_size(cortecs_type type) {
    return (type & CORTECS_TYPE_SIZE_MASK) >> CORTECS_TYPE_SIZE_SHIFT;
}
cortecs_type_finalizer_index cortecs_type_index(cortecs_type type) {
    return (cortecs_type_finalizer_index)((type & CORTECS_TYPE_INDEX_MASK) >> CORTECS_TYPE_INDEX_SHIFT);
}