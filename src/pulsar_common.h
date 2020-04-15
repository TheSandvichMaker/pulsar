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

typedef int32_t  b32;

typedef float    f32;
typedef double   f64;

typedef uint8_t  u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t   s8;
typedef int16_t  s16;
typedef int32_t  s32;
typedef int64_t  s64;

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

#define sizeof_member(struct_type, member) sizeof((cast(struct_type*) 0)->member)

#ifndef NULL
#ifdef __cplusplus
#define NULL ((void*)0)
#else
#define NULL 0
#endif
#endif

#define define_safe_truncate(big, small, min, max)             \
    inline small safe_truncate_##big##_##small##(big source) { \
        assert(source >= min && source <= max);                \
        small result = cast(small) source;                     \
        return result;                                         \
    }

define_safe_truncate(u64, u32, 0, UINT32_MAX);
define_safe_truncate(u64, u16, 0, UINT16_MAX);
define_safe_truncate(u64, u8 , 0, UINT8_MAX);
define_safe_truncate(u32, u16, 0, UINT16_MAX);
define_safe_truncate(u32, u8 , 0, UINT8_MAX);
define_safe_truncate(u16, u8 , 0, UINT8_MAX);

define_safe_truncate(s64, s32, INT32_MIN, INT32_MAX);
define_safe_truncate(s64, s16, INT16_MIN, INT16_MAX);
define_safe_truncate(s64, s8 , INT8_MIN , INT8_MAX);
define_safe_truncate(s32, s16, INT16_MIN, INT16_MAX);
define_safe_truncate(s32, s8 , INT8_MIN , INT8_MAX);
define_safe_truncate(s16, s8 , INT8_MIN , INT8_MAX);

define_safe_truncate(f64, f32, -FLT_MAX , FLT_MAX);

#undef define_safe_truncate

#define define_saturating_cast(big, small, min, max)             \
    inline small saturating_cast_##big##_##small##(big source) { \
        small result = cast(small) CLAMP(source, min, max);      \
        return result;                                           \
    }

define_saturating_cast(u64, u32, 0, UINT32_MAX);
define_saturating_cast(u64, u16, 0, UINT16_MAX);
define_saturating_cast(u64, u8 , 0, UINT8_MAX);
define_saturating_cast(u32, u16, 0, UINT16_MAX);
define_saturating_cast(u32, u8 , 0, UINT8_MAX);
define_saturating_cast(u16, u8 , 0, UINT8_MAX);

define_saturating_cast(s64, s32, INT32_MIN, INT32_MAX);
define_saturating_cast(s64, s16, INT16_MIN, INT16_MAX);
define_saturating_cast(s64, s8 , INT8_MIN , INT8_MAX);
define_saturating_cast(s32, s16, INT16_MIN, INT16_MAX);
define_saturating_cast(s32, s8 , INT8_MIN , INT8_MAX);
define_saturating_cast(s16, s8 , INT8_MIN , INT8_MAX);

define_saturating_cast(f64, f32, -FLT_MAX , FLT_MAX);

#undef define_saturating_cast

#define checked_access(pointer, member) ((pointer) ? (pointer)->member : 0)
#define SWAP(a, b) { auto t = a; a = b; b = t; }

#define introspect(...)

#define PULSAR_MEMORY_MALLOC_ALLOCATOR 1
#include "pulsar_memory.h"
#include "pulsar_memory_arena.h"

#include "math.h"
#include "string.h"
#include "file_io.h"

#include "pulsar_asset_pack_file_format.h"

#endif /* WIN32_MAIN_H */
