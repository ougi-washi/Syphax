// Syphax-Web - Ougi Washi

#ifndef S_TYPES_H
#define S_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>

#define SYPHAX_TYPES
#ifdef SYPHAX_TYPES
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
#endif

#define s_assert(expr) if (!(expr)) { fprintf(stderr, "Assertion failed: %s\n", #expr); assert(0); }
#define s_assertf(expr, ...) if (!(expr)) { fprintf(stderr, "Assertion failed: %s\n", #expr); fprintf(stderr, __VA_ARGS__); assert(0); }

#endif // S_TYPES_H
