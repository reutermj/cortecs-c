#ifndef CORTECS_TYPES_TYPES_H
#define CORTECS_TYPES_TYPES_H

#include <stdint.h>

typedef uint8_t U8;
typedef uint16_t U16;
typedef uint32_t U32;
typedef uint64_t U64;

typedef int8_t I8;
typedef int16_t I16;
typedef int32_t I32;
typedef int64_t I64;

typedef float F32;
typedef double F64;

#define TYPE_PARAM_T U8
#include "ptr.template.h"

#define TYPE_PARAM_T U16
#include "ptr.template.h"

#define TYPE_PARAM_T U32
#include "ptr.template.h"

#define TYPE_PARAM_T U64
#include "ptr.template.h"

#define TYPE_PARAM_T I8
#include "ptr.template.h"

#define TYPE_PARAM_T I16
#include "ptr.template.h"

#define TYPE_PARAM_T I32
#include "ptr.template.h"

#define TYPE_PARAM_T I64
#include "ptr.template.h"

#define TYPE_PARAM_T F32
#include "ptr.template.h"

#define TYPE_PARAM_T F64
#include "ptr.template.h"

#endif