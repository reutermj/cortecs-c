#ifndef CORTECS_TYPES_TYPES_H
#define CORTECS_TYPES_TYPES_H

#include <cortecs/mangle.h>
#include <stdint.h>

// ====================================================================================================================
// Core Number Types
// ====================================================================================================================
typedef uint8_t CN(Cortecs, U8);
#define TYPE_PARAM_T CN(Cortecs, U8)
#include "array.template.h"
#include "ptr.template.h"

typedef uint16_t CN(Cortecs, U16);
#define TYPE_PARAM_T CN(Cortecs, U16)
#include "array.template.h"
#include "ptr.template.h"

typedef uint32_t CN(Cortecs, U32);
#define TYPE_PARAM_T CN(Cortecs, U32)
#include "array.template.h"
#include "ptr.template.h"

typedef uint64_t CN(Cortecs, U64);
#define TYPE_PARAM_T CN(Cortecs, U64)
#include "array.template.h"
#include "ptr.template.h"

typedef int8_t CN(Cortecs, I8);
#define TYPE_PARAM_T CN(Cortecs, I8)
#include "array.template.h"
#include "ptr.template.h"

typedef int16_t CN(Cortecs, I16);
#define TYPE_PARAM_T CN(Cortecs, I16)
#include "array.template.h"
#include "ptr.template.h"

typedef int32_t CN(Cortecs, I32);
#define TYPE_PARAM_T CN(Cortecs, I32)
#include "array.template.h"
#include "ptr.template.h"

typedef int64_t CN(Cortecs, I64);
#define TYPE_PARAM_T CN(Cortecs, I64)
#include "array.template.h"
#include "ptr.template.h"

typedef float CN(Cortecs, F32);
#define TYPE_PARAM_T CN(Cortecs, F32)
#include "array.template.h"
#include "ptr.template.h"

typedef double CN(Cortecs, F64);
#define TYPE_PARAM_T CN(Cortecs, F64)
#include "array.template.h"
#include "ptr.template.h"

#endif