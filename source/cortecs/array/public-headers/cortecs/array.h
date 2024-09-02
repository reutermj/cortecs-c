#ifndef COMMON_ARRAYS_H
#define COMMON_ARRAYS_H

#include <common.h>
#include <stdint.h>

#define cortecs_array_forward_declare(TYPE) \
    typedef struct CONCAT(cortecs_array_, TYPE) CONCAT(cortecs_array_, TYPE)

#define cortecs_array_define(TYPE)        \
    struct CONCAT(cortecs_array_, TYPE) { \
        uint32_t size;                    \
        TYPE elements[];                  \
    }

#define cortecs_array_declare(TYPE)      \
    cortecs_array_forward_declare(TYPE); \
    cortecs_array_define(TYPE);

#define cortecs_array(TYPE) CONCAT(cortecs_array_, TYPE)*

cortecs_array_declare(int8_t);
cortecs_array_declare(int16_t);
cortecs_array_declare(int32_t);
cortecs_array_declare(int64_t);
cortecs_array_declare(uint8_t);
cortecs_array_declare(uint16_t);
cortecs_array_declare(uint32_t);
cortecs_array_declare(uint64_t);

typedef struct {
    uint32_t size;
    uint8_t elements[];
} cortecs_array_void;

#endif