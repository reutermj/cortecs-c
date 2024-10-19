#ifndef TYPE_PARAM_T
#error "Expected TYPE_PARAM_T to be define"
#endif

#include <cortecs/mangle.h>

typedef TYPE_PARAM_T *cortecs_name(Cortecs, Ptr, cortecs_type_params(TYPE_PARAM_T));

#undef TYPE_PARAM_T