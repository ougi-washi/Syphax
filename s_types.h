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
#include <stdio.h>
#include <stdlib.h>

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

#ifndef SYPHAX_MALLOC
#define SYPHAX_MALLOC malloc
#endif

#ifndef SYPHAX_REALLOC
#define SYPHAX_REALLOC realloc
#endif

#ifndef SYPHAX_FREE
#define SYPHAX_FREE free
#endif

#ifndef SYPHAX_ABORT
#define SYPHAX_ABORT abort
#endif

#ifndef s_malloc
#define s_malloc(_size) SYPHAX_MALLOC((_size))
#endif

#ifndef s_realloc
#define s_realloc(_ptr, _size) SYPHAX_REALLOC((_ptr), (_size))
#endif

#ifndef s_free
#define s_free(_ptr) SYPHAX_FREE((_ptr))
#endif

#ifndef s_abort
#define s_abort() SYPHAX_ABORT()
#endif

#define s_assert(_expr) \
    do { \
        if (!(_expr)) { \
            fprintf(stderr, "[%s:%d] assertion failed: %s\n", __FILE__, __LINE__, #_expr); \
            s_abort(); \
        } \
    } while (0)

#define s_assertf(_expr, ...) \
    do { \
        if (!(_expr)) { \
            fprintf(stderr, "[%s:%d] assertion failed: %s\n", __FILE__, __LINE__, #_expr); \
            fprintf(stderr, __VA_ARGS__); \
            s_abort(); \
        } \
    } while (0)

#endif // S_TYPES_H
