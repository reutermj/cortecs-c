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

cortecs_type cortecs_type_register_finalizer_impl(cortecs_type_param(T), cortecs_type_finalizer finalizer) {
    cortecs_type_arg(T).index = next_type_index;
    registered_types[next_type_index] = finalizer;
    next_type_index++;
    return cortecs_type_arg(T);
}

cortecs_type_finalizer cortecs_type_lookup_finalizer(cortecs_type_finalizer_index index) {
    return registered_types[index];
}