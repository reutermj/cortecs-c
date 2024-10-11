#include <cortecs/finalizer.h>

// Define registered type info
// reserved types:
//   0: NOOP on collection
//   1: Decrement pointer on collection (mostly useful for arrays of pointers)
#define TYPE_BITS 16
#define MAX_REGISTERED_TYPES (1 << TYPE_BITS)
static cortecs_finalizer_metadata registered_types[MAX_REGISTERED_TYPES];
static uint32_t next_type_index;

cortecs_finalizer_define(uint32_t);

void cortecs_finalizer_init() {
    next_type_index = 1;
}

cortecs_finalizer_index cortecs_finalizer_register_impl(cortecs_finalizer_metadata metadata) {
    cortecs_finalizer_index index = next_type_index;
    next_type_index++;
    registered_types[index] = metadata;
    return index;
}

cortecs_finalizer_metadata cortecs_finalizer_get(cortecs_finalizer_index index) {
    return registered_types[index];
}