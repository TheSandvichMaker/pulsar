#ifndef HANDMADE_INTRINSICS_H
#define HANDMADE_INTRINSICS_H

#include <cmath>

//
// TODO: Convert all of these to platform-efficient versions
// and remove math.h
//

inline s32 sign_of(s32 value) {
    s32 result = value >= 0 ? 1 : -1;
    return result;
}

inline f32 sign_of(f32 value) {
    f32 result = value >= 0.0f ? 1.0f : -1.0f;
    return result;
}

inline f32 square_root(f32 f) {
    return sqrtf(f);
}

inline f32 fract(f32 f) {
    f32 ignored;
    return modf(f, &ignored);
}

#undef abs
#define abs my_abs
inline f32 my_abs(f32 f) {
    return fabs(f);
}

inline u32 rotate_left(u32 value, s32 amount) {
#if COMPILER_MSVC
    u32 result = _rotl(value, amount);
#else
    amount &= 31;
    u32 result = ((value <<  amount) | (value >> (32 - amount)));
#endif

    return result;
}

inline u32 rotate_right(u32 value, s32 amount) {
#if COMPILER_MSVC
    u32 result = _rotr(value, amount);
#else
    amount &= 31;
    u32 result = ((value >>  amount) | (value << (32 - amount)));
#endif
    return result;
}

inline s32 round_f32_to_i32(f32 f) {
    return (s32)roundf(f);
}

inline u32 round_f32_to_u32(f32 f) {
    return (u32)roundf(f);
}

inline s32 floor_f32_to_i32(f32 f) {
    return (s32)floorf(f);
}

inline s32 ceil_f32_to_i32(f32 f) {
    return (s32)ceilf(f);
}

inline s32 truncate_f32_to_i32(f32 f) {
    return (s32)f;
}

#ifdef sin
#undef sin
#endif
#define sin my_sin
inline f32 my_sin(f32 a) {
    return sinf(a);
}

#ifdef cos
#undef cos
#endif
#define cos my_cos
inline f32 my_cos(f32 a) {
    return cosf(a);
}

#ifdef atan2
#undef atan2
#endif
#define atan2 my_atan2
inline f32 my_atan2(f32 y, f32 x) {
    return atan2f(y, x);
}

struct BitScanResult {
    u32 found;
    u32 index;
};

inline BitScanResult find_least_significant_set_bit(u32 value) {
    BitScanResult result = {};

#if COMPILER_MSVC
    result.found = _BitScanForward((unsigned long*)&result.index, value);
#else
    for (u32 test = 0; test < 32; test++) {
        if (value & (1 << test)) {
            result.index = test;
            result.found = true;
            break;
        }
    }
#endif
    return result;
}

#endif
