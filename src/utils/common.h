#pragma once

// Basic Types, Math Functions, etc.

typedef unsigned char       u8;
typedef unsigned short      u16;
typedef unsigned int        u32;
typedef unsigned long long  u64;

typedef signed char         s8;
typedef short               s16;
typedef int                 s32;
typedef long long           s64;

typedef float               f32;
typedef double              f64;

static inline f32 lerp(f32 a, f32 b, f32 t)
{
    return a + (b - a) * t;
}

static inline s32 floor_(f32 x)
{
    return (x < 0 && x != (float)((s32)(x))) ? (s32)(x) - 1 : (s32)(x);
}

static inline void clamp_(s32& num, s32 min, s32 max)
{
    if (num > max) num = max;
    if (num < min) num = min;
}

static inline s32 abs_(s32 x)
{
    return (x < 0) ? -x : x;
}

static inline f32 abs_(f32 x)
{
    return (x < 0) ? -x : x;
}
