#ifndef TYPE_PARAM_T
#error "Expected TYPE_PARAM_T to be define"
#endif

#include <cortecs/mangle.h>
#include <stdint.h>

struct CN(Cortecs, Array, CT(TYPE_PARAM_T)) {
    uint32_t size;
    TYPE_PARAM_T elements[];
};
