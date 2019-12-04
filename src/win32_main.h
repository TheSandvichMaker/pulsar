#ifndef WIN32_MAIN_H
#define WIN32_MAIN_H

#include <cstdint>
#include <cstddef>
#include <cfloat>
#include <assert.h>

#define INVALID_CODE_PATH assert(!"Invalid Code Path");
#define INVALID_DEFAULT_CASE default: { assert(!"Invalid Default Case"); } break;
#define NOT_IMPLEMENTED assert(!"Not Implemented");

#define ARRAY_COUNT(array) (sizeof(array) / sizeof((array)[0]))

typedef int32_t b32;

typedef float  f32;
typedef double f64;

typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

#define internal static
#define global static
#define local_persist static

#define cast(type) (type)

#ifndef NULL
#define NULL 0
#endif

#endif /* WIN32_MAIN_H */
