#ifndef CORTECS_TYPES_TYPES_H
#define CORTECS_TYPES_TYPES_H

#include <cortecs/mangle.h>
#include <stdint.h>

typedef uint8_t CN(Cortecs, U8);
typedef uint16_t CN(Cortecs, U16);
typedef uint32_t CN(Cortecs, U32);
typedef uint64_t CN(Cortecs, U64);

typedef int8_t CN(Cortecs, I8);
typedef int16_t CN(Cortecs, I16);
typedef int32_t CN(Cortecs, I32);
typedef int64_t CN(Cortecs, I64);

typedef float CN(Cortecs, F32);
typedef double CN(Cortecs, F64);

#define TYPE_PARAM_T CN(Cortecs, U8)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, U16)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, U32)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, U64)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, I8)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, I16)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, I32)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, I64)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, F32)
#include "ptr.template.h"

#define TYPE_PARAM_T CN(Cortecs, F64)
#include "ptr.template.h"

#endif