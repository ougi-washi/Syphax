// Syphax-Web - Ougi Washi

#ifndef S_TYPES_H
#define S_TYPES_H

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include <assert.h>
#include <stdio.h>
#include <math.h>

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

#define SYPHAX_MATH
#ifdef SYPHAX_MATH
typedef struct { f32 x, y; } s_vec2;
typedef struct { f32 x, y, z; } s_vec3;
typedef struct { f32 x, y, z, w; } s_vec4;
typedef struct { f32 m[3][3]; } s_mat3;
typedef struct { f32 m[4][4]; } s_mat4;
#define s_vec(_vec_size, ...) (s_vec##_vec_size){ __VA_ARGS__ }
#define s_min(a, b) ((a) < (b) ? (a) : (b))
#define s_max(a, b) ((a) > (b) ? (a) : (b))
#define PI 3.14159265359

#define s_vec2(_x, _y) (s_vec(2, _x, _y))
#define s_vec2_add(a, b) (s_vec2(a.x + b.x, a.y + b.y))
#define s_vec2_sub(a, b) (s_vec2(a.x - b.x, a.y - b.y))
#define s_vec2_mul(a, b) (s_vec2(a.x * b.x, a.y * b.y))
#define s_vec2_div(a, b) (s_vec2(a.x / b.x, a.y / b.y))
#define s_vec2_dot(a, b) (a.x * b.x + a.y * b.y)
#define s_vec2_cross(a, b) (a.x * b.y - a.y * b.x)
#define s_vec2_length(a) (sqrtf(a.x * a.x + a.y * a.y))
#define s_vec2_normalize(a) (s_vec2_div(a, s_vec2_length(a)))
#define s_vec2_lerp(a, b, t) (s_vec2_add(s_vec2_mul(a, 1.0f - t), s_vec2_mul(b, t)))
#define s_vec2_reflect(a, n) (s_vec2_sub(a, s_vec2_mul(n, 2.0f * s_vec2_dot(a, n))))
#define s_vec2_from_angle(a) (s_vec2(cosf(a), sinf(a)))
#define s_vec2_to_angle(a) (atan2f(a.y, a.x))

#define s_vec3(_x, _y, _z) (s_vec(3, _x, _y, _z))
#define s_vec3_add(a, b) (s_vec3(a.x + b.x, a.y + b.y, a.z + b.z))
#define s_vec3_sub(a, b) (s_vec3(a.x - b.x, a.y - b.y, a.z - b.z))
#define s_vec3_mul(a, b) (s_vec3(a.x * b.x, a.y * b.y, a.z * b.z))
#define s_vec3_div(a, b) (s_vec3(a.x / b.x, a.y / b.y, a.z / b.z))
#define s_vec3_dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z)
#define s_vec3_cross(a, b) (s_vec3(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x))
#define s_vec3_length(a) (sqrtf(a.x * a.x + a.y * a.y + a.z * a.z))
#define s_vec3_normalize(a) (s_vec3_div(a, s_vec3_length(a)))
#define s_vec3_lerp(a, b, t) (s_vec3_add(s_vec3_mul(a, 1.0f - t), s_vec3_mul(b, t)))
#define s_vec3_reflect(a, n) (s_vec3_sub(a, s_vec3_mul(n, 2.0f * s_vec3_dot(a, n))))
#define s_vec3_from_angle(a) (s_vec3(cosf(a), sinf(a), 0.0f))
#define s_vec3_to_angle(a) (atan2f(a.y, a.x))

#define s_vec4(_x, _y, _z, _w) (s_vec(4, _x, _y, _z, _w))
#define s_vec4_add(a, b) (s_vec4(a.x + b.x, a.y + b.y, a.z + b.z, a.w + b.w))
#define s_vec4_sub(a, b) (s_vec4(a.x - b.x, a.y - b.y, a.z - b.z, a.w - b.w))
#define s_vec4_mul(a, b) (s_vec4(a.x * b.x, a.y * b.y, a.z * b.z, a.w * b.w))
#define s_vec4_div(a, b) (s_vec4(a.x / b.x, a.y / b.y, a.z / b.z, a.w / b.w))
#define s_vec4_dot(a, b) (a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w)
#define s_vec4_cross(a, b) (s_vec4(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x, a.w * b.x - a.x * b.w))
#define s_vec4_length(a) (sqrtf(a.x * a.x + a.y * a.y + a.z * a.z + a.w * a.w))
#define s_vec4_normalize(a) (s_vec4_div(a, s_vec4_length(a)))
#define s_vec4_lerp(a, b, t) (s_vec4_add(s_vec4_mul(a, 1.0f - t), s_vec4_mul(b, t)))
#define s_vec4_reflect(a, n) (s_vec4_sub(a, s_vec4_mul(n, 2.0f * s_vec4_dot(a, n))))
#define s_vec4_from_angle(a) (s_vec4(cosf(a), sinf(a), 0.0f, 0.0f))
#define s_vec4_to_angle(a) (atan2f(a.y, a.x))

#define s_mat3(_m00, _m01, _m02, _m10, _m11, _m12, _m20, _m21, _m22) \
	(s_mat3){ \
		.m = { \
			{ _m00, _m01, _m02 }, \
			{ _m10, _m11, _m12 }, \
			{ _m20, _m21, _m22 } \
		} \
	}
#define s_mat3_identity (s_mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f))
static inline s_mat3 s_mat3_mul(s_mat3 a, s_mat3 b) {
    s_mat3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 3; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}
#define s_mat3_transpose(a) (s_mat3( \
	a.m[0][0], a.m[1][0], a.m[2][0], \
	a.m[0][1], a.m[1][1], a.m[2][1], \
	a.m[0][2], a.m[1][2], a.m[2][2] \
))
#define s_mat3_inverse(a) (s_mat3( \
	a.m[0][0], a.m[1][0], a.m[2][0], \
	a.m[0][1], a.m[1][1], a.m[2][1], \
	a.m[0][2], a.m[1][2], a.m[2][2] \
))
#define s_mat3_translate(a, v) (s_mat3_mul(a, s_mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f, v.x, v.y, v.z)))
#define s_mat3_rotate(a, v) (s_mat3_mul(a, s_mat3(cosf(v), -sinf(v), 0.0f, sinf(v), cosf(v), 0.0f, 0.0f, 0.0f, 1.0f)))
#define s_mat3_scale(a, v) (s_mat3_mul(a, s_mat3(v.x, 0.0f, 0.0f, 0.0f, v.y, 0.0f, 0.0f, 0.0f, v.z)))
#define s_mat3_shear(a, v) (s_mat3_mul(a, s_mat3(1.0f, v.x, v.y, v.z, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f)))

#define s_mat4(_m00, _m01, _m02, _m03, _m10, _m11, _m12, _m13, _m20, _m21, _m22, _m23, _m30, _m31, _m32, _m33) \
	(s_mat4){ \
		.m = { \
			{ _m00, _m01, _m02, _m03 }, \
			{ _m10, _m11, _m12, _m13 }, \
			{ _m20, _m21, _m22, _m23 }, \
			{ _m30, _m31, _m32, _m33 } \
		} \
	}
#define s_mat4_identity (s_mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f))
static inline s_mat4 s_mat4_mul(s_mat4 a, s_mat4 b) {
    s_mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += a.m[i][k] * b.m[k][j];
            }
        }
    }
    return result;
}
#define s_mat4_transpose(a) (s_mat4( \
	a.m[0][0], a.m[1][0], a.m[2][0], a.m[3][0], \
	a.m[0][1], a.m[1][1], a.m[2][1], a.m[3][1], \
	a.m[0][2], a.m[1][2], a.m[2][2], a.m[3][2], \
	a.m[0][3], a.m[1][3], a.m[2][3], a.m[3][3] \
))
#define s_mat4_inverse(a) (s_mat4( \
	a.m[0][0], a.m[1][0], a.m[2][0], a.m[3][0], \
	a.m[0][1], a.m[1][1], a.m[2][1], a.m[3][1], \
	a.m[0][2], a.m[1][2], a.m[2][2], a.m[3][2], \
	a.m[0][3], a.m[1][3], a.m[2][3], a.m[3][3] \
))
#define s_mat4_translate(a, v) (s_mat4_mul(a, s_mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, v.x, v.y, v.z, 1.0f)))
#define s_mat4_rotate(a, v) (s_mat4_mul(a, s_mat4(cosf(v), -sinf(v), 0.0f, 0.0f, sinf(v), cosf(v), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f)))
#define s_mat4_scale(a, v) (s_mat4_mul(a, s_mat4(v.x, 0.0f, 0.0f, 0.0f, 0.0f, v.y, 0.0f, 0.0f, 0.0f, 0.0f, v.z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f)))

#endif // SYPHAX_MATH
#endif // SYPHAX_TYPES

#define s_assert(expr) if (!(expr)) { fprintf(stderr, "Assertion failed: %s\n", #expr); assert(0); }
#define s_assertf(expr, ...) if (!(expr)) { fprintf(stderr, "Assertion failed: %s\n", #expr); fprintf(stderr, __VA_ARGS__); assert(0); }

#endif // S_TYPES_H
