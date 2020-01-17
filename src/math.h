#ifndef HANDMADE_MATH_H
#define HANDMADE_MATH_H

#include "intrinsics.h"

#define PI_32  3.14159265359f
#define TAU_32 6.28318530717f
#define DEG_TO_RAD (TAU_32/360.0f)
#define RAD_TO_DEG (360.0f/TAU_32)
#define F32_MAX FLT_MAX

//
// NOTE: Scalar operations
//

inline f32 deg_to_rad(f32 deg) {
    f32 rad = deg * DEG_TO_RAD;
    return rad;
}

inline f32 rad_to_deg(f32 rad) {
    f32 deg = rad * RAD_TO_DEG;
    return deg;
}

inline f32 square(f32 a) {
    return a * a;
}

inline s32 square(s32 a) {
    return a * a;
}

inline u32 square(u32 a) {
    return a * a;
}

inline f32 lerp(f32 a, f32 b, f32 t) {
    return a * (1.0f - t) + b * t;
}

#undef max
inline f32 max(f32 a, f32 b) {
    return a > b ? a : b;
}

inline f64 max(f64 a, f64 b) {
    return a > b ? a : b;
}

#undef min
inline f32 min(f32 a, f32 b) {
    return a < b ? a : b;
}

inline f64 min(f64 a, f64 b) {
    return a < b ? a : b;
}

#undef clamp
inline f32 clamp(f32 n, f32 lo, f32 hi) {
    return max(lo, min(hi, n));
}

inline f64 clamp(f64 n, f64 lo, f64 hi) {
    return max(lo, min(hi, n));
}

#define clamp01(n) clamp((n), 0, 1)

inline f32 map_to_range(f32 t, f32 min, f32 max) {
    f32 result = 0.0f;
    f32 range = max - min;
    if (range != 0.0f) {
        result = (t - min) / range;
    }
    return result;
}

inline f32 clamp01_map_to_range(f32 t, f32 min, f32 max) {
     f32 result = clamp01(map_to_range(t, min, max));
     return result;
}

inline f32 safe_ratio_n(f32 numerator, f32 divisor, f32 n) {
    f32 result = n;

    if (divisor != 0.0f) {
        result = numerator / divisor;
    }

    return result;
}

inline f32 safe_ratio_0(f32 numerator, f32 divisor) {
    f32 result = safe_ratio_n(numerator, divisor, 0.0f);
    return result;
}

inline f32 safe_ratio_1(f32 numerator, f32 divisor) {
    f32 result = safe_ratio_n(numerator, divisor, 1.0f);
    return result;
}

inline f32 neighborhood_distance(f32 a, f32 b, f32 period = TAU_32) {
    f32 d0 = abs(a - b);
    f32 d1 = abs(a - period*sign_of(a) - b);
    f32 result = min(d0, d1);
    return result;
}

//
// NOTE: Types
//

union v2 {
    struct {
        f32 x, y;
    };
    struct {
        f32 u, v;
    };
    f32 e[2];
};

union v3 {
    struct {
        f32 x, y, z;
    };
    struct {
        f32 r, g, b;
    };
    struct {
        f32 u, v, w;
    };
    struct {
        v2 xy;
        f32 ignored0_;
    };
    struct {
        f32 ignored1_;
        v2 yz;
    };
    struct {
        v2 rg;
        f32 ignored2_;
    };
    struct {
        f32 ignored3_;
        v2 gb;
    };
    struct {
        v2 uv;
        f32 ignored4_;
    };
    struct {
        f32 ignored5_;
        v2 vw;
    };
    f32 e[3];
};

union v4 {
    struct {
        f32 x, y, z, w;
    };
    struct {
        f32 r, g, b, a;
    };
    struct {
        v3 xyz;
        f32 ignored0_;
    };
    struct {
        f32 ignored1_;
        v3 yzw;
    };
    struct {
        v2 xy;
        v2 zw;
    };
    struct {
        f32 ignored2_;
        v2 yz;
        f32 ignored3_;
    };
    struct {
        v3 rgb;
        f32 ignored4_;
    };
    struct {
        f32 ignored5_;
        v3 gba;
    };
    struct {
        v2 rg;
        v2 ba;
    };
    struct {
        f32 ignored6_;
        v2 gb;
        f32 ignored7_;
    };
    f32 e[4];
};

// @Note: The convention is that the rectangle bounds are [min,max): max is exclusive.

struct AxisAlignedBox2 {
    v2 min;
    v2 max;
};

struct AxisAlignedBox3 {
    v3 min;
    v3 max;
};

//
//
//

inline b32 vectors_equal(v2 a, v2 b) {
    b32 result = (a.x == b.x) && (a.y == b.y);
    return result;
}

//
// NOTE: v2
//

inline v2 vec2(f32 x, f32 y) {
    v2 result;
    result.x = x;
    result.y = y;
    return result;
}

inline v2 vec2(u32 x, u32 y) {
    v2 result;
    result.x = (f32)x;
    result.y = (f32)y;
    return result;
}

inline v2 vec2(s32 x, s32 y) {
    v2 result;
    result.x = (f32)x;
    result.y = (f32)y;
    return result;
}

inline v2 vec2(f32 s) {
    v2 result;
    result.x = s;
    result.y = s;
    return result;
}

inline b32 any(v2 v) {
    b32 result = v.x && v.y;
    return result;
}

inline v2 operator - (v2 a) {
    v2 result;
    result.x = -a.x;
    result.y = -a.y;
    return result;
}

#define IMPLEMENT_VECTOR_OPERATORS(op) \
    inline v2 operator op (v2 a, v2 b) { \
        v2 result; \
        result.x = a.x op b.x; \
        result.y = a.y op b.y; \
        return result; \
    } \
    inline v2 &operator op##= (v2& a, v2 b) { \
        a = a op b; \
        return a; \
    }

#define IMPLEMENT_SCALAR_OPERATORS(op) \
    inline v2 operator op (v2 a, f32 b) { \
        v2 result; \
        result.x = a.x op b; \
        result.y = a.y op b; \
        return result; \
    } \
    inline v2 operator op (f32 a, v2 b) { \
        v2 result; \
        result.x = a op b.x; \
        result.y = a op b.y; \
        return result; \
    } \
    inline v2 &operator op##= (v2& a, f32 b) { \
        a = a op b; \
        return a; \
    }

// IMPLEMENT_SCALAR_OPERATORS(+)
// IMPLEMENT_SCALAR_OPERATORS(-)
IMPLEMENT_VECTOR_OPERATORS(+)
IMPLEMENT_VECTOR_OPERATORS(-)
IMPLEMENT_SCALAR_OPERATORS(*)
IMPLEMENT_SCALAR_OPERATORS(/)
IMPLEMENT_VECTOR_OPERATORS(*)
IMPLEMENT_VECTOR_OPERATORS(/)

#undef IMPLEMENT_VECTOR_OPERATORS
#undef IMPLEMENT_SCALAR_OPERATORS

inline v2 perp(v2 v) {
    v2 result = vec2(-v.y, v.x);
    return result;
}

inline v2 perp_clockwise(v2 v) {
    v2 result = vec2(v.y, -v.x);
    return result;
}

inline v2 mul(v2 a, v2 b) {
    v2 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    return result;
}

inline v2 square(v2 v) {
    v2 result;
    result.x = v.x * v.x;
    result.y = v.y * v.y;
    return result;
}

inline v2 lerp(v2 a, v2 b, f32 t) {
    return a * (1.0f - t) + b * t;
}

inline f32 dot(v2 a, v2 b) {
    return a.x * b.x + a.y * b.y;
}

inline v2 reflect(v2 v, v2 n) {
    return v - 2.0f*n*dot(v, n);
}

inline f32 length_sq(v2 v) {
    f32 result = dot(v, v);
    return result;
}

inline f32 length(v2 v) {
    f32 result = square_root(dot(v, v));
    return result;
}

inline v2 normalize(v2 v) {
    assert(length(v) != 0.0f);
    return v / length(v);
}

inline v2 normalize_or_zero(v2 v) {
    v2 result = {};

    f32 l_sq = length_sq(v);
    if (l_sq > square(0.0001f)) {
        result = v * (1.0f / square_root(l_sq));
    }
    return result;
}

inline v2 min(v2 a, v2 b) {
    v2 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    return result;
}

inline v2 min(v2 a, f32 b) {
    v2 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    return result;
}

inline v2 min(f32 a, v2 b) {
    v2 result;
    result.x = min(a, b.x);
    result.y = min(a, b.y);
    return result;
}

inline v2 max(v2 a, v2 b) {
    v2 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    return result;
}

inline v2 max(v2 a, f32 b) {
    v2 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    return result;
}

inline v2 max(f32 a, v2 b) {
    v2 result;
    result.x = max(a, b.x);
    result.y = max(a, b.y);
    return result;
}

inline v2 clamp(v2 n, v2 lo, v2 hi) {
    v2 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    return result;
}

inline v2 clamp(v2 n, f32 lo, v2 hi) {
    v2 result;
    result.x = max(lo, min(hi.x, n.x));
    result.y = max(lo, min(hi.y, n.y));
    return result;
}

inline v2 clamp(v2 n, v2 lo, f32 hi) {
    v2 result;
    result.x = max(lo.x, min(hi, n.x));
    result.y = max(lo.y, min(hi, n.y));
    return result;
}

inline v2 clamp(v2 n, f32 lo, f32 hi) {
    v2 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    return result;
}

inline v2 arm2(f32 angle) {
    v2 result;
    result.x = cos(angle);
    result.y = sin(angle);
    return result;
}

inline v2 rotate(v2 v, v2 cos_sin) {
    v2 result = vec2(v.x*cos_sin.x - v.y*cos_sin.y, v.x*cos_sin.y + v.y*cos_sin.x);
    return result;
}

inline v2 rotate_clockwise(v2 v, v2 cos_sin) {
    v2 result = vec2(v.x*cos_sin.x + v.y*cos_sin.y, -v.x*cos_sin.y + v.y*cos_sin.x);
    return result;
}

inline b32 is_in_region(v2 region, v2 p) {
    b32 result = abs(p.x) <= 0.5f*region.x && abs(p.y) <= 0.5f*region.y;
    return result;
}

inline v2 clamp_to_region(v2 region, v2 p) {
    v2 result = clamp(p, min(vec2(0.0f, 0.0f), -region), max(vec2(0.0f, 0.0f), region));
    return result;
}

//
// NOTE: AxisAlignedBox2
//

/*
 I'm saying AABs start bottom left, wound counter clockwise, regarding the naming of corners.
    D --------- C
    |           |
    |           |
    A --------- B
*/

inline v2 corner_a(AxisAlignedBox2 aab) {
    v2 result = aab.min;
    return result;
}

inline v2 corner_b(AxisAlignedBox2 aab) {
    v2 result = vec2(aab.max.x, aab.min.y);
    return result;
}

inline v2 corner_c(AxisAlignedBox2 aab) {
    v2 result = aab.max;
    return result;
}

inline v2 corner_d(AxisAlignedBox2 aab) {
    v2 result = vec2(aab.min.x, aab.max.y);
    return result;
}

inline v2 get_dim(AxisAlignedBox2 aab) {
    v2 result = aab.max - aab.min;
    return result;
}

inline v2 get_min_corner(AxisAlignedBox2 aab) {
    v2 result = aab.min;
    return result;
}

inline v2 get_max_corner(AxisAlignedBox2 aab) {
    v2 result = aab.max;
    return result;
}

inline v2 get_center(AxisAlignedBox2 aab) {
    v2 result = 0.5f * (aab.min + aab.max);
    return result;
}

inline f32 get_aspect_ratio(AxisAlignedBox2 aab) {
    v2 dim = get_dim(aab);
    f32 aspect_ratio = dim.x/dim.y;
    return aspect_ratio;
}

inline AxisAlignedBox2 aab_min_max(v2 min, v2 max) {
    AxisAlignedBox2 result;
    result.min = min;
    result.max = max;
    return result;
}

inline AxisAlignedBox2 aab_center_half_dim(v2 center, v2 half_dim) {
    AxisAlignedBox2 result;
    result.min = center - half_dim;
    result.max = center + half_dim;
    return result;
}

inline AxisAlignedBox2 aab_center_dim(v2 center, v2 dim) {
    AxisAlignedBox2 result;
    result.min = center - dim * 0.5f;
    result.max = center + dim * 0.5f;
    return result;
}

inline AxisAlignedBox2 aab_min_dim(v2 min, v2 dim) {
    AxisAlignedBox2 result;
    result.min = min;
    result.max = min + dim;
    return result;
}

inline AxisAlignedBox2 multiply_dimensions(AxisAlignedBox2 aab, v2 mul) {
    AxisAlignedBox2 result;
    result.min = aab.min * mul;
    result.max = aab.max * mul;
    return result;
}

inline AxisAlignedBox2 grow_by_radius(AxisAlignedBox2 aab, v2 r_dim) {
    AxisAlignedBox2 result;
    result.min = aab.min - r_dim;
    result.max = aab.max + r_dim;
    return result;
}

inline AxisAlignedBox2 grow_by_diameter(AxisAlignedBox2 aab, v2 d_dim) {
    AxisAlignedBox2 result;
    result.min = aab.min - d_dim * 0.5f;
    result.max = aab.max + d_dim * 0.5f;
    return result;
}

inline AxisAlignedBox2 offset(AxisAlignedBox2 a, v2 offset) {
    AxisAlignedBox2 result;
    result.min = a.min + offset;
    result.max = a.max + offset;
    return result;
}

inline b32 is_in_aab(AxisAlignedBox2 aab, v2 test) {
    b32 result = (test.x >= aab.min.x && test.x < aab.max.x) &&
                 (test.y >= aab.min.y && test.y < aab.max.y);
    return result;
}

inline b32 aab_contained_in_aab(AxisAlignedBox2 outer, AxisAlignedBox2 inner) {
    b32 result = (outer.min.x <= inner.min.x && outer.max.x >= inner.max.x) &&
                 (outer.min.y <= inner.min.y && outer.max.y >= inner.max.y);
    return result;
}

inline b32 aab_intersect(AxisAlignedBox2 a, AxisAlignedBox2 b) {
    b32 result = !(b.max.x <= a.min.x ||
                   b.min.x >= a.max.x ||
                   b.max.y <= a.min.y ||
                   b.min.y >= a.max.y);
    return result;
}

inline AxisAlignedBox2 aab_sum(AxisAlignedBox2 a, AxisAlignedBox2 b) {
    AxisAlignedBox2 result;
    result.min = a.min + b.min;
    result.max = a.max + b.max;
    return result;
}

inline v2 get_barycentric(AxisAlignedBox2 aab, v2 p) {
    v2 result;
    result.x = safe_ratio_0(p.x - aab.min.x, aab.max.x - aab.min.x);
    result.y = safe_ratio_0(p.y - aab.min.y, aab.max.y - aab.min.y);
    return result;
}

inline AxisAlignedBox2 aab_union(AxisAlignedBox2 a, AxisAlignedBox2 b) {
    AxisAlignedBox2 result;
    result.min.x = min(a.min.x, b.min.x);
    result.min.y = min(a.min.y, b.min.y);
    result.max.x = max(a.max.x, b.max.x);
    result.max.y = max(a.max.y, b.max.y);
    return result;
}

inline AxisAlignedBox2 intersect(AxisAlignedBox2 a, AxisAlignedBox2 b) {
    AxisAlignedBox2 result;
    result.min.x = max(a.min.x, b.min.x);
    result.min.y = max(a.min.y, b.min.y);
    result.max.x = min(a.max.x, b.max.x);
    result.max.y = min(a.max.y, b.max.y);
    return result;
}

inline AxisAlignedBox2 grow_to_contain(AxisAlignedBox2 aab, v2 p) {
    AxisAlignedBox2 result = aab;
    result.min = min(result.min, p);
    result.max = max(result.max, p);
    return result;
}

inline AxisAlignedBox2 inverted_infinity_aab2() {
    AxisAlignedBox2 result;
    result.min.x = result.min.y =  F32_MAX;
    result.max.x = result.max.y = -F32_MAX;
    return result;
}

//
// NOTE: AxisAlignedBox2i
//

struct AxisAlignedBox2i {
    s32 min_x, min_y;
    s32 max_x, max_y;
};

inline AxisAlignedBox2i aab_union(AxisAlignedBox2i a, AxisAlignedBox2i b) {
    AxisAlignedBox2i result;
    result.min_x = MIN(a.min_x, b.min_x);
    result.min_y = MIN(a.min_y, b.min_y);
    result.max_x = MAX(a.max_x, b.max_x);
    result.max_y = MAX(a.max_y, b.max_y);
    return result;
}

inline AxisAlignedBox2i intersect(AxisAlignedBox2i a, AxisAlignedBox2i b) {
    AxisAlignedBox2i result;
    result.min_x = MAX(a.min_x, b.min_x);
    result.min_y = MAX(a.min_y, b.min_y);
    result.max_x = MIN(a.max_x, b.max_x);
    result.max_y = MIN(a.max_y, b.max_y);
    return result;
}

inline s32 get_area(AxisAlignedBox2i box) {
    s32 w = (box.max_x - box.min_x);
    s32 h = (box.max_y - box.min_y);
    s32 area = 0;
    if (w > 0 && h > 0) {
        area = w*h;
    }
    return area;
}

inline AxisAlignedBox2i inverted_infinity_aab2i() {
    AxisAlignedBox2i result;
    result.min_x = result.min_y =  INT32_MAX;
    result.max_x = result.max_y = -INT32_MAX;
    return result;
}

//
// NOTE: v3
//

inline v3 vec3(f32 x, f32 y, f32 z) {
    v3 result;
    result.x = x;
    result.y = y;
    result.z = z;
    return result;
}

inline v3 vec3(f32 s) {
    v3 result;
    result.x = s;
    result.y = s;
    result.z = s;
    return result;
}

inline v3 vec3(v2 v2, f32 z) {
    v3 result;
    result.x = v2.x;
    result.y = v2.y;
    result.z = z;
    return result;
}

inline v3 vec3(f32 x, v2 v2) {
    v3 result;
    result.x = x;
    result.y = v2.x;
    result.z = v2.y;
    return result;
}

inline b32 any(v3 v) {
    b32 result = v.x && v.y && v.z;
    return result;
}

inline v3 operator - (v3 a) {
    v3 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    return result;
}

#define IMPLEMENT_VECTOR_OPERATORS(op) \
    inline v3 operator op (v3 a, v3 b) { \
        v3 result; \
        result.x = a.x op b.x; \
        result.y = a.y op b.y; \
        result.z = a.z op b.z; \
        return result; \
    } \
    inline v3 &operator op##= (v3& a, v3 b) { \
        a = a op b; \
        return a; \
    }

#define IMPLEMENT_SCALAR_OPERATORS(op) \
    inline v3 operator op (v3 a, f32 b) { \
        v3 result; \
        result.x = a.x op b; \
        result.y = a.y op b; \
        result.z = a.z op b; \
        return result; \
    } \
    inline v3 operator op (f32 a, v3 b) { \
        v3 result; \
        result.x = a op b.x; \
        result.y = a op b.y; \
        result.z = a op b.z; \
        return result; \
    } \
    inline v3 &operator op##= (v3& a, f32 b) { \
        a = a op b; \
        return a; \
    }

// IMPLEMENT_SCALAR_OPERATORS(+)
// IMPLEMENT_SCALAR_OPERATORS(-)
IMPLEMENT_VECTOR_OPERATORS(+)
IMPLEMENT_VECTOR_OPERATORS(-)
IMPLEMENT_SCALAR_OPERATORS(*)
IMPLEMENT_SCALAR_OPERATORS(/)
IMPLEMENT_VECTOR_OPERATORS(*)
IMPLEMENT_VECTOR_OPERATORS(/)

#undef IMPLEMENT_VECTOR_OPERATORS
#undef IMPLEMENT_SCALAR_OPERATORS

inline v3 shift(v3 v, f32 s) {
    v3 result;
    result.x += s;
    result.y += s;
    result.z += s;
    return result;
}

inline v3 mul(v3 a, v3 b) {
    v3 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    return result;
}

inline v3 square(v3 v) {
    v3 result;
    result.x = v.x * v.x;
    result.y = v.y * v.y;
    result.z = v.z * v.z;
    return result;
}

inline v3 square_root(v3 v) {
    v3 result;
    result.x = square_root(v.x);
    result.y = square_root(v.y);
    result.z = square_root(v.z);
    return result;
}

inline v3 lerp(v3 a, v3 b, f32 t) {
    return a * (1.0f - t) + b * t;
}

inline f32 dot(v3 a, v3 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z;
}

inline v3 cross(v3 a, v3 b) {
    return vec3(
        a.y*b.z - a.z*b.y,
        a.z*b.x - a.x*b.z,
        a.x*b.y - a.y*b.x
    );
}

inline v3 reflect(v3 a, v3 b) {
    return a - 2.0f * dot(a, b) * b;
}

inline f32 length_sq(v3 v) {
    f32 result = dot(v, v);
    return result;
}

inline f32 length(v3 v) {
    f32 result = square_root(dot(v, v));
    return result;
}

inline v3 normalize(v3 v) {
    v3 result = v / length(v);
    return result;
}

inline v3 normalize_or_zero(v3 v) {
    v3 result = {};

    f32 l_sq = length_sq(v);
    if (l_sq > square(0.0001f)) {
        result = v * (1.0f / square_root(l_sq));
    }
    return result;
}

inline v3 min(v3 a, v3 b) {
    v3 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    result.z = min(a.z, b.z);
    return result;
}

inline v3 min(v3 a, f32 b) {
    v3 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    result.z = min(a.z, b);
    return result;
}

inline v3 min(f32 a, v3 b) {
    v3 result;
    result.x = min(a, b.x);
    result.y = min(a, b.y);
    result.z = min(a, b.z);
    return result;
}

inline v3 max(v3 a, v3 b) {
    v3 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    result.z = max(a.z, b.z);
    return result;
}

inline v3 max(v3 a, f32 b) {
    v3 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    result.z = max(a.z, b);
    return result;
}

inline v3 max(f32 a, v3 b) {
    v3 result;
    result.x = max(a, b.x);
    result.y = max(a, b.y);
    result.z = max(a, b.z);
    return result;
}

inline v3 clamp(v3 n, v3 lo, v3 hi) {
    v3 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    result.z = max(lo.z, min(hi.z, n.z));
    return result;
}

inline v3 clamp(v3 n, f32 lo, v3 hi) {
    v3 result;
    result.x = max(lo, min(hi.x, n.x));
    result.y = max(lo, min(hi.y, n.y));
    result.z = max(lo, min(hi.z, n.z));
    return result;
}

inline v3 clamp(v3 n, v3 lo, f32 hi) {
    v3 result;
    result.x = max(lo.x, min(hi, n.x));
    result.y = max(lo.y, min(hi, n.y));
    result.z = max(lo.z, min(hi, n.z));
    return result;
}

inline v3 clamp(v3 n, f32 lo, f32 hi) {
    v3 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    result.z = max(lo, min(hi, n.z));
    return result;
}

//
// NOTE: AxisAlignedBox3
//

inline v3 get_dim(AxisAlignedBox3 aab) {
    v3 result = aab.max - aab.min;
    return result;
}

inline v3 get_min_corner(AxisAlignedBox3 aab) {
    v3 result = aab.min;
    return result;
}

inline v3 get_max_corner(AxisAlignedBox3 aab) {
    v3 result = aab.max;
    return result;
}

inline v3 get_center(AxisAlignedBox3 aab) {
    v3 result = 0.5f * (aab.min + aab.max);
    return result;
}

inline AxisAlignedBox3 aab_min_max(v3 min, v3 max) {
    AxisAlignedBox3 result;
    result.min = min;
    result.max = max;
    return result;
}

inline AxisAlignedBox3 aab_center_half_dim(v3 center, v3 half_dim) {
    AxisAlignedBox3 result;
    result.min = center - half_dim;
    result.max = center + half_dim;
    return result;
}

inline AxisAlignedBox3 aab_center_dim(v3 center, v3 dim) {
    AxisAlignedBox3 result;
    result.min = center - dim * 0.5f;
    result.max = center + dim * 0.5f;
    return result;
}

inline AxisAlignedBox3 aab_min_dim(v3 min, v3 dim) {
    AxisAlignedBox3 result;
    result.min = min;
    result.max = min + dim;
    return result;
}

inline AxisAlignedBox3 multiply_dimensions(AxisAlignedBox3 aab, v3 mul) {
    AxisAlignedBox3 result;
    result.min = aab.min * mul;
    result.max = aab.max * mul;
    return result;
}

inline AxisAlignedBox3 grow_by_radius(AxisAlignedBox3 aab, v3 r_dim) {
    AxisAlignedBox3 result;
    result.min = aab.min - r_dim;
    result.max = aab.max + r_dim;
    return result;
}

inline AxisAlignedBox3 grow_by_diameter(AxisAlignedBox3 aab, v3 d_dim) {
    AxisAlignedBox3 result;
    result.min = aab.min - d_dim * 0.5f;
    result.max = aab.max + d_dim * 0.5f;
    return result;
}

inline AxisAlignedBox3 offset(AxisAlignedBox3 a, v3 offset) {
    AxisAlignedBox3 result;
    result.min = a.min + offset;
    result.max = a.max + offset;
    return result;
}

inline b32 is_in_aab(AxisAlignedBox3 aab, v3 test) {
    b32 result = (test.x >= aab.min.x && test.x < aab.max.x) &&
                  (test.y >= aab.min.y && test.y < aab.max.y) &&
                  (test.z >= aab.min.z && test.z < aab.max.z);
    return result;
}

inline b32 aab_contained_in_aab(AxisAlignedBox3 outer, AxisAlignedBox3 inner) {
    b32 result = (outer.min.x >= inner.min.x && outer.max.x <= inner.max.x) &&
                  (outer.min.y >= inner.min.y && outer.max.y <= inner.max.y) &&
                  (outer.min.z >= inner.min.z && outer.max.z <= inner.max.z);
    return result;
}

inline b32 aab_intersect(AxisAlignedBox3 a, AxisAlignedBox3 b) {
    b32 result = !(b.max.x <= a.min.x ||
                    b.min.x >= a.max.x ||
                    b.max.y <= a.min.y ||
                    b.min.y >= a.max.y ||
                    b.max.z <= a.min.z ||
                    b.min.z >= a.max.z);
    return result;
}

inline v3 get_barycentric(AxisAlignedBox3 aab, v3 p) {
    v3 result;
    result.x = safe_ratio_0(p.x - aab.min.x, aab.max.x - aab.min.x);
    result.y = safe_ratio_0(p.y - aab.min.y, aab.max.y - aab.min.y);
    result.z = safe_ratio_0(p.z - aab.min.z, aab.max.z - aab.min.z);
    return result;
}

inline AxisAlignedBox2 to_aab2_xy(AxisAlignedBox3 aab) {
    AxisAlignedBox2 result;
    result.min = aab.min.xy;
    result.max = aab.max.xy;
    return result;
}

//
// NOTE: v4
//

inline v4 vec4(f32 x, f32 y, f32 z, f32 w) {
    v4 result;
    result.x = x;
    result.y = y;
    result.z = z;
    result.w = w;
    return result;
}

inline v4 vec4(f32 s) {
    v4 result;
    result.x = s;
    result.y = s;
    result.z = s;
    result.w = s;
    return result;
}

inline v4 vec4(v2 v2, f32 z, f32 w) {
    v4 result;
    result.x = v2.x;
    result.y = v2.y;
    result.z = z;
    result.w = w;
    return result;
}

inline v4 vec4(f32 x, v2 v2, f32 w) {
    v4 result;
    result.x = x;
    result.y = v2.x;
    result.z = v2.y;
    result.w = w;
    return result;
}

inline v4 vec4(f32 x, f32 y, v2 v2) {
    v4 result;
    result.x = x;
    result.y = y;
    result.z = v2.x;
    result.w = v2.y;
    return result;
}

inline v4 vec4(v2 a, v2 b) {
    v4 result;
    result.x = a.x;
    result.y = a.y;
    result.z = b.x;
    result.w = b.y;
    return result;
}

inline v4 vec4(v3 v3, f32 w) {
    v4 result;
    result.x = v3.x;
    result.y = v3.y;
    result.z = v3.z;
    result.w = w;
    return result;
}

inline v4 vec4(f32 x, v3 v3) {
    v4 result;
    result.x = x;
    result.y = v3.x;
    result.z = v3.y;
    result.w = v3.z;
    return result;
}

inline b32 any(v4 v) {
    b32 result = v.x && v.y && v.z && v.w;
    return result;
}

inline v4 operator - (v4 a) {
    v4 result;
    result.x = -a.x;
    result.y = -a.y;
    result.z = -a.z;
    result.w = -a.w;
    return result;
}

#define IMPLEMENT_VECTOR_OPERATORS(op) \
    inline v4 operator op (v4 a, v4 b) { \
        v4 result; \
        result.x = a.x op b.x; \
        result.y = a.y op b.y; \
        result.z = a.z op b.z; \
        result.w = a.w op b.w; \
        return result; \
    } \
    inline v4 &operator op##= (v4& a, v4 b) { \
        a = a op b; \
        return a; \
    }

#define IMPLEMENT_SCALAR_OPERATORS(op) \
    inline v4 operator op (v4 a, f32 b) { \
        v4 result; \
        result.x = a.x op b; \
        result.y = a.y op b; \
        result.z = a.z op b; \
        result.w = a.w op b; \
        return result; \
    } \
    inline v4 operator op (f32 a, v4 b) { \
        v4 result; \
        result.x = a op b.x; \
        result.y = a op b.y; \
        result.z = a op b.z; \
        result.w = a op b.w; \
        return result; \
    } \
    inline v4 &operator op##= (v4& a, f32 b) { \
        a = a op b; \
        return a; \
    }

// IMPLEMENT_SCALAR_OPERATORS(+)
// IMPLEMENT_SCALAR_OPERATORS(-)
IMPLEMENT_VECTOR_OPERATORS(+)
IMPLEMENT_VECTOR_OPERATORS(-)
IMPLEMENT_SCALAR_OPERATORS(*)
IMPLEMENT_SCALAR_OPERATORS(/)
IMPLEMENT_VECTOR_OPERATORS(*)
IMPLEMENT_VECTOR_OPERATORS(/)

#undef IMPLEMENT_VECTOR_OPERATORS
#undef IMPLEMENT_SCALAR_OPERATORS

inline v4 mul(v4 a, v4 b) {
    v4 result;
    result.x = a.x * b.x;
    result.y = a.y * b.y;
    result.z = a.z * b.z;
    result.w = a.w * b.w;
    return result;
}

inline v4 square(v4 v) {
    v4 result;
    result.x = v.x * v.x;
    result.y = v.y * v.y;
    result.z = v.z * v.z;
    result.w = v.w * v.w;
    return result;
}

inline v4 lerp(v4 a, v4 b, f32 t) {
    return a * (1.0f - t) + b * t;
}

inline f32 dot(v4 a, v4 b) {
    return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

inline v4 reflect(v4 a, v4 b) {
    return a - 2.0f * dot(a, b) * b;
}

inline f32 length_sq(v4 v) {
    f32 result = dot(v, v);
    return result;
}

inline f32 length(v4 v) {
    f32 result = square_root(dot(v, v));
    return result;
}

inline v4 min(v4 a, v4 b) {
    v4 result;
    result.x = min(a.x, b.x);
    result.y = min(a.y, b.y);
    result.z = min(a.z, b.z);
    result.w = min(a.w, b.w);
    return result;
}

inline v4 min(v4 a, f32 b) {
    v4 result;
    result.x = min(a.x, b);
    result.y = min(a.y, b);
    result.z = min(a.z, b);
    result.w = min(a.w, b);
    return result;
}

inline v4 min(f32 a, v4 b) {
    v4 result;
    result.x = min(a, b.x);
    result.y = min(a, b.y);
    result.z = min(a, b.z);
    result.w = min(a, b.w);
    return result;
}

inline v4 max(v4 a, v4 b) {
    v4 result;
    result.x = max(a.x, b.x);
    result.y = max(a.y, b.y);
    result.z = max(a.z, b.z);
    result.w = max(a.w, b.w);
    return result;
}

inline v4 max(v4 a, f32 b) {
    v4 result;
    result.x = max(a.x, b);
    result.y = max(a.y, b);
    result.z = max(a.z, b);
    result.w = max(a.w, b);
    return result;
}

inline v4 max(f32 a, v4 b) {
    v4 result;
    result.x = max(a, b.x);
    result.y = max(a, b.y);
    result.z = max(a, b.z);
    result.w = max(a, b.w);
    return result;
}

inline v4 clamp(v4 n, v4 lo, v4 hi) {
    v4 result;
    result.x = max(lo.x, min(hi.x, n.x));
    result.y = max(lo.y, min(hi.y, n.y));
    result.z = max(lo.z, min(hi.z, n.z));
    result.w = max(lo.w, min(hi.w, n.w));
    return result;
}

inline v4 clamp(v4 n, f32 lo, v4 hi) {
    v4 result;
    result.x = max(lo, min(hi.x, n.x));
    result.y = max(lo, min(hi.y, n.y));
    result.z = max(lo, min(hi.z, n.z));
    result.w = max(lo, min(hi.w, n.w));
    return result;
}

inline v4 clamp(v4 n, v4 lo, f32 hi) {
    v4 result;
    result.x = max(lo.x, min(hi, n.x));
    result.y = max(lo.y, min(hi, n.y));
    result.z = max(lo.z, min(hi, n.z));
    result.w = max(lo.w, min(hi, n.w));
    return result;
}

inline v4 clamp(v4 n, f32 lo, f32 hi) {
    v4 result;
    result.x = max(lo, min(hi, n.x));
    result.y = max(lo, min(hi, n.y));
    result.z = max(lo, min(hi, n.z));
    result.w = max(lo, min(hi, n.w));
    return result;
}

inline v3 srgb_to_linear(v3 c) {
    v3 result;

    result = square(c);

    return result;
}

inline v3 linear_to_srgb(v3 c) {
    v3 result;

    result = square_root(c);

    return result;
}

inline v4 srgb_to_linear(v4 c) {
    v4 result;

    result.rgb = srgb_to_linear(c.rgb);
    result.a = c.a;

    return result;
}

inline v4 linear_to_srgb(v4 c) {
    v4 result;

    result.rgb = linear_to_srgb(c.rgb);
    result.a = c.a;

    return result;
}


inline v4 srgb255_to_linear1(v4 c) {
    v4 result;

    result.rgb = srgb_to_linear(c.rgb * (1.0f/255.0f));
    result.a = c.a / 255.0f;

    return result;
}

inline v4 linear1_to_srgb255(v4 c) {
    v4 result;

    result.rgb = linear_to_srgb(c.rgb) * 255.0f;
    result.a = c.a * 255.0f;

    return result;
}

#endif
