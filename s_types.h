/**
 * Syphax - Ougi Washi
 *
 * Easy to use types and math library for C.
 *
 * MIT License
 * Copyright (c) 2025-2026 Jed - Ougi Washi https://github.com/ougi-washi/Syphax
 */

#ifndef S_TYPES_H
#define S_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>

typedef bool b8;
typedef int8_t i8;
typedef uint8_t u8;
typedef int16_t i16;
typedef uint16_t u16;
typedef int32_t i32;
typedef uint32_t u32;
typedef int64_t i64;
typedef uint64_t u64;
typedef float f32;
typedef double f64;
typedef char c8;
typedef unsigned char uc8;
typedef size_t sz;

#define s_assert(expr) if (!(expr))         { fprintf(stderr, "[%s: %d] Assertion failed: %s\n", __FILE__, __LINE__, #expr); assert(0); }
#define s_assertf(expr, ...) if (!(expr))   { fprintf(stderr, "[%s: %d] Assertion failed: %s\n", __FILE__, __LINE__, #expr); fprintf(stderr, __VA_ARGS__); assert(0); }

#endif // S_TYPES_H
