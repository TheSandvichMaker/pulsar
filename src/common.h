#ifndef WIN32_MAIN_H
#define WIN32_MAIN_H

#include <stdint.h>
#include <stddef.h>
#include <float.h>
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

#define ABS(val) ((val) < 0 ? -(val) : (val))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define CLAMP(x, lo, hi) (MAX((lo), MIN((hi), (x))))

#define align_pow2(value, align) (((value) + ((align) - 1)) & ~((align) - 1))
#define align4(value) ((value + 3) & ~3)
#define align8(value) ((value + 7) & ~7)
#define align16(value) ((value + 15) & ~15)

#define KILOBYTES(num) ((num) * 1024)
#define MEGABYTES(num) (KILOBYTES(num) * 1024)
#define GIGABYTES(num) (MEGABYTES(num) * 1024)
#define TERABYTES(num) (GIGABYTES(num) * 1024)

#define offset_of(st, m) ((size_t)&(((st*)0)->m))

#ifndef NULL
#ifdef __cplusplus
#define NULL ((void*)0)
#else
#define NULL 0
#endif
#endif

inline u32 safe_truncate_u64u32(u64 source) {
    assert(source <= UINT32_MAX);
    u32 result = cast(u32) source;
    return result;
}

#define introspect(...)

#endif /* WIN32_MAIN_H */
