#ifndef COMMON_ARRAYS_H
#define COMMON_ARRAYS_H

#include <common.h>
#include <stdint.h>

#define array_declare(TYPE) \
    typedef struct {        \
        uint32_t size;      \
        TYPE elements[];    \
    } CONCAT(array_, TYPE);

#define array(TYPE) CONCAT(array_, TYPE)*

array_declare(int8_t);
array_declare(int16_t);
array_declare(int32_t);
array_declare(int64_t);
array_declare(uint8_t);
array_declare(uint16_t);
array_declare(uint32_t);
array_declare(uint64_t);

typedef struct {
    uint32_t size;

} array_void;

#endif