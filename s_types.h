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
#define s_min(_a_vec, _b_vec) ((_a_vec) < (_b_vec) ? (_a_vec) : (_b_vec))
#define s_max(_a_vec, _b_vec) ((_a_vec) > (_b_vec) ? (_a_vec) : (_b_vec))
#define PI 3.14159265359

#define s_vec2(_x, _y)                  (s_vec(2, _x, _y))
#define s_vec2_add(_vec_a, _vec_b)      (s_vec2(_vec_a.x + _vec_b.x, _vec_a.y + _vec_b.y))
#define s_vec2_sub(_vec_a, _vec_b)      (s_vec2(_vec_a.x - _vec_b.x, _vec_a.y - _vec_b.y))
#define s_vec2_mul(_vec_a, _vec_b)      (s_vec2(_vec_a.x * _vec_b.x, _vec_a.y * _vec_b.y))
#define s_vec2_div(_vec_a, _vec_b)      (s_vec2(_vec_a.x / _vec_b.x, _vec_a.y / _vec_b.y))
#define s_vec2_muls(_vec_a, _s)         (s_vec2(_vec_a.x * _s, _vec_a.y * _s))
#define s_vec2_divs(_vec_a, _s)         (s_vec2(_vec_a.x / _s, _vec_a.y / _s))
#define s_vec2_dot(_vec_a, _vec_b)      (_vec_a.x * _vec_b.x + _vec_a.y * _vec_b.y)
#define s_vec2_cross(_vec_a, _vec_b)    (_vec_a.x * _vec_b.y - _vec_a.y * _vec_b.x)
#define s_vec2_length(_vec_a)           (sqrtf(_vec_a.x * _vec_a.x + _vec_a.y * _vec_a.y))
static inline s_vec2 s_vec2_normalize(s_vec2 _vec_a) {
    f32 len = s_vec2_length(_vec_a);
    if (len == 0.0f) return s_vec2(0.0f, 0.0f);
    return s_vec2(_vec_a.x / len, _vec_a.y / len);
}
#define s_vec2_lerp(_vec_a, _vec_b, t)  (s_vec2(_vec_a.x + t * (_vec_b.x - _vec_a.x), _vec_a.y + t * (_vec_b.y - _vec_a.y)))
#define s_vec2_reflect(_vec_a, _n)      (s_vec2_sub(_vec_a, s_vec2_muls(_n, 2.0f * s_vec2_dot(_vec_a, _n))))
#define s_vec2_from_angle(_vec_a)       (s_vec2(cosf(_vec_a), sinf(_vec_a)))
#define s_vec2_to_angle(_vec_a)         (atan2f(_vec_a.y, _vec_a.x))

#define s_vec3(_x, _y, _z)              (s_vec(3, _x, _y, _z))
#define s_vec3_add(_vec_a, _vec_b)      (s_vec3(_vec_a.x + _vec_b.x, _vec_a.y + _vec_b.y, _vec_a.z + _vec_b.z))
#define s_vec3_sub(_vec_a, _vec_b)      (s_vec3(_vec_a.x - _vec_b.x, _vec_a.y - _vec_b.y, _vec_a.z - _vec_b.z))
#define s_vec3_mul(_vec_a, _vec_b)      (s_vec3(_vec_a.x * _vec_b.x, _vec_a.y * _vec_b.y, _vec_a.z * _vec_b.z))
#define s_vec3_div(_vec_a, _vec_b)      (s_vec3(_vec_a.x / _vec_b.x, _vec_a.y / _vec_b.y, _vec_a.z / _vec_b.z))
#define s_vec3_muls(_vec_a, _s)         (s_vec3(_vec_a.x * _s, _vec_a.y * _s, _vec_a.z * _s))
#define s_vec3_divs(_vec_a, _s)         (s_vec3(_vec_a.x / _s, _vec_a.y / _s, _vec_a.z / _s))
#define s_vec3_dot(_vec_a, _vec_b)      (_vec_a.x * _vec_b.x + _vec_a.y * _vec_b.y + _vec_a.z * _vec_b.z)
#define s_vec3_cross(_vec_a, _vec_b)    (s_vec3(_vec_a.y * _vec_b.z - _vec_a.z * _vec_b.y, _vec_a.z * _vec_b.x - _vec_a.x * _vec_b.z, _vec_a.x * _vec_b.y - _vec_a.y * _vec_b.x))
#define s_vec3_length(_vec_a)           (sqrtf(_vec_a.x * _vec_a.x + _vec_a.y * _vec_a.y + _vec_a.z * _vec_a.z))
static inline s_vec3 s_vec3_normalize(s_vec3 _vec_a) {
    f32 len = s_vec3_length(_vec_a);
    if (len == 0.0f) return s_vec3(0.0f, 0.0f, 0.0f);
    return s_vec3(_vec_a.x / len, _vec_a.y / len, _vec_a.z / len);
}
#define s_vec3_lerp(_vec_a, _vec_b, t)  (s_vec3(_vec_a.x + t * (_vec_b.x - _vec_a.x), _vec_a.y + t * (_vec_b.y - _vec_a.y), _vec_a.z + t * (_vec_b.z - _vec_a.z)))
#define s_vec3_reflect(_vec_a, _n)      (s_vec3_sub(_vec_a, s_vec3_muls(_n, 2.0f * s_vec3_dot(_vec_a, _n))))
#define s_vec3_from_angle(_vec_a)       (s_vec3(cosf(_vec_a), sinf(_vec_a), 0.0f))
#define s_vec3_to_angle(_vec_a)         (atan2f(_vec_a.y, _vec_a.x))

#define s_vec4(_x, _y, _z, _w)          (s_vec(4, _x, _y, _z, _w))
#define s_vec4_add(_vec_a, _vec_b)      (s_vec4(_vec_a.x + _vec_b.x, _vec_a.y + _vec_b.y, _vec_a.z + _vec_b.z, _vec_a.w + _vec_b.w))
#define s_vec4_sub(_vec_a, _vec_b)      (s_vec4(_vec_a.x - _vec_b.x, _vec_a.y - _vec_b.y, _vec_a.z - _vec_b.z, _vec_a.w - _vec_b.w))
#define s_vec4_mul(_vec_a, _vec_b)      (s_vec4(_vec_a.x * _vec_b.x, _vec_a.y * _vec_b.y, _vec_a.z * _vec_b.z, _vec_a.w * _vec_b.w))
#define s_vec4_div(_vec_a, _vec_b)      (s_vec4(_vec_a.x / _vec_b.x, _vec_a.y / _vec_b.y, _vec_a.z / _vec_b.z, _vec_a.w / _vec_b.w))
#define s_vec4_muls(_vec_a, _s)         (s_vec4(_vec_a.x * _s, _vec_a.y * _s, _vec_a.z * _s, _vec_a.w * _s))
#define s_vec4_divs(_vec_a, _s)         (s_vec4(_vec_a.x / _s, _vec_a.y / _s, _vec_a.z / _s, _vec_a.w / _s))
#define s_vec4_dot(_vec_a, _vec_b)      (_vec_a.x * _vec_b.x + _vec_a.y * _vec_b.y + _vec_a.z * _vec_b.z + _vec_a.w * _vec_b.w)
#define s_vec4_cross(_vec_a, _vec_b)    (s_vec4(_vec_a.y * _vec_b.z - _vec_a.z * _vec_b.y, _vec_a.z * _vec_b.x - _vec_a.x * _vec_b.z, _vec_a.x * _vec_b.y - _vec_a.y * _vec_b.x, 0.0f))
#define s_vec4_length(_vec_a)           (sqrtf(_vec_a.x * _vec_a.x + _vec_a.y * _vec_a.y + _vec_a.z * _vec_a.z + _vec_a.w * _vec_a.w))
static inline s_vec4 s_vec4_normalize(s_vec4 _vec_a) {
    f32 len = s_vec4_length(_vec_a);
    if (len == 0.0f) return s_vec4(0.0f, 0.0f, 0.0f, 0.0f);
    return s_vec4(_vec_a.x / len, _vec_a.y / len, _vec_a.z / len, _vec_a.w / len);
}
#define s_vec4_lerp(_vec_a, _vec_b, t)  (s_vec4(_vec_a.x + t * (_vec_b.x - _vec_a.x), _vec_a.y + t * (_vec_b.y - _vec_a.y), _vec_a.z + t * (_vec_b.z - _vec_a.z), _vec_a.w + t * (_vec_b.w - _vec_a.w)))
#define s_vec4_reflect(_vec_a, _n)      (s_vec4_sub(_vec_a, s_vec4_muls(_n, 2.0f * s_vec4_dot(_vec_a, _n))))
#define s_vec4_from_angle(_vec_a)       (s_vec4(cosf(_vec_a), sinf(_vec_a), 0.0f, 0.0f))
#define s_vec4_to_angle(_vec_a)         (atan2f(_vec_a.y, _vec_a.x))

#define s_mat3(_m00, _m01, _m02, _m10, _m11, _m12, _m20, _m21, _m22) \
	(s_mat3){ \
		.m = { \
			{ _m00, _m01, _m02 }, \
			{ _m10, _m11, _m12 }, \
			{ _m20, _m21, _m22 } \
		} \
	}
#define s_mat3_identity (s_mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f))
static inline s_mat3 s_mat3_mul(const s_mat3* _mat_a, const s_mat3* _mat_b) {
    s_mat3 result;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 3; k++) {
                result.m[i][j] += _mat_a->m[i][k] * _mat_b->m[k][j];
            }
        }
    }
    return result;
}
#define s_mat3_transpose(_mat) (s_mat3( \
	_mat.m[0][0], _mat.m[1][0], _mat.m[2][0], \
	_mat.m[0][1], _mat.m[1][1], _mat.m[2][1], \
	_mat.m[0][2], _mat.m[1][2], _mat.m[2][2] \
))
static inline s_mat3 s_mat3_inverse(const s_mat3* _mat) {
    s_mat3 inv = *_mat;
    f32 aug[3][6];
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) aug[i][j] = inv.m[i][j];
        for (int j = 0; j < 3; j++) aug[i][j + 3] = (i == j) ? 1.0f : 0.0f;
    }
    for (int i = 0; i < 3; i++) {
        f32 pivot = aug[i][i];
        if (pivot == 0.0f) return s_mat3_identity;
        for (int j = 0; j < 6; j++) aug[i][j] /= pivot;
        for (int k = 0; k < 3; k++) {
            if (k != i) {
                f32 factor = aug[k][i];
                for (int j = 0; j < 6; j++) aug[k][j] -= factor * aug[i][j];
            }
        }
    }
    for (int i = 0; i < 3; i++)
        for (int j = 0; j < 3; j++)
            inv.m[i][j] = aug[i][j + 3];
    return inv;
}
#define s_mat3_translate(_mat, _v)        (s_mat3_mul(_mat, &(s_mat3){ { {1.0f, 0.0f, _v.x}, {0.0f, 1.0f, _v.y}, {0.0f, 0.0f, 1.0f} } }))
#define s_mat3_set_translation(_mat, _v)  { _mat.m[0][2] = _v.x; _mat.m[1][2] = _v.y; }
#define s_mat3_get_translation(_mat)      (s_vec2(_mat.m[0][2], _mat.m[1][2]))
#define s_mat3_rotate(_mat, _angle)       (s_mat3_mul(_mat, &(s_mat3){ { {cosf(_angle), -sinf(_angle), 0.0f}, {sinf(_angle), cosf(_angle), 0.0f}, {0.0f, 0.0f, 1.0f} } }))
#define s_mat3_set_rotation(_mat, _angle) { _mat.m[0][0] = cosf(_angle); _mat.m[0][1] = -sinf(_angle); _mat.m[1][0] = sinf(_angle); _mat.m[1][1] = cosf(_angle); }
#define s_mat3_get_rotation(_mat)         (atan2f(_mat.m[1][0], _mat.m[0][0]))
#define s_mat3_scale(_mat, _v)            (s_mat3_mul(_mat, &(s_mat3){ { {_v.x, 0.0f, 0.0f}, {0.0f, _v.y, 0.0f}, {0.0f, 0.0f, 1.0f} } }))
#define s_mat3_set_scale(_mat, _v)        { _mat.m[0][0] = _v.x; _mat.m[1][1] = _v.y; }
#define s_mat3_get_scale(_mat)            (s_vec2(_mat.m[0][0], _mat.m[1][1]))
#define s_mat3_shear(_mat, _v)            (s_mat3_mul(_mat, &(s_mat3){ { {1.0f, _v.x, 0.0f}, {_v.y, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} } }))
#define s_mat3_set_shear(_mat, _v)        { _mat.m[0][1] = _v.x; _mat.m[1][0] = _v.y; }
#define s_mat3_get_shear(_mat)            (s_vec2(_mat.m[0][1], _mat.m[1][0]))

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
static inline s_mat4 s_mat4_mul(const s_mat4* _mat_a, const s_mat4* _mat_b) {
    s_mat4 result;
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            result.m[i][j] = 0.0f;
            for (int k = 0; k < 4; k++) {
                result.m[i][j] += _mat_a->m[i][k] * _mat_b->m[k][j];
            }
        }
    }
    return result;
}
#define s_mat4_transpose(_mat) (s_mat4( \
	_mat.m[0][0], _mat.m[1][0], _mat.m[2][0], _mat.m[3][0], \
	_mat.m[0][1], _mat.m[1][1], _mat.m[2][1], _mat.m[3][1], \
	_mat.m[0][2], _mat.m[1][2], _mat.m[2][2], _mat.m[3][2], \
	_mat.m[0][3], _mat.m[1][3], _mat.m[2][3], _mat.m[3][3] \
))
static inline s_mat4 s_mat4_inverse(const s_mat4* _mat) {
    s_mat4 inv = *_mat;
    f32 aug[4][8];
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) aug[i][j] = inv.m[i][j];
        for (int j = 0; j < 4; j++) aug[i][j + 4] = (i == j) ? 1.0f : 0.0f;
    }
    for (int i = 0; i < 4; i++) {
        f32 pivot = aug[i][i];
        if (pivot == 0.0f) return s_mat4_identity;
        for (int j = 0; j < 8; j++) aug[i][j] /= pivot;
        for (int k = 0; k < 4; k++) {
            if (k != i) {
                f32 factor = aug[k][i];
                for (int j = 0; j < 8; j++) aug[k][j] -= factor * aug[i][j];
            }
        }
    }
    for (int i = 0; i < 4; i++)
        for (int j = 0; j < 4; j++)
            inv.m[i][j] = aug[i][j + 4];
    return inv;
}
#define s_mat4_translate(_mat, _v)       (s_mat4_mul(_mat, &(s_mat4){ { {1.0f, 0.0f, 0.0f, _v.x}, {0.0f, 1.0f, 0.0f, _v.y}, {0.0f, 0.0f, 1.0f, _v.z}, {0.0f, 0.0f, 0.0f, 1.0f} } }))
#define s_mat4_set_translation(_mat, _v) { _mat.m[0][3] = _v.x; _mat.m[1][3] = _v.y; _mat.m[2][3] = _v.z; }
#define s_mat4_get_translation(_mat)     (s_vec3(_mat.m[0][3], _mat.m[1][3], _mat.m[2][3]))
#define s_mat4_rotate_z(_mat, _angle)    (s_mat4_mul(_mat, &(s_mat4){ { {cosf(_angle), -sinf(_angle), 0.0f, 0.0f}, {sinf(_angle), cosf(_angle), 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } }))
#define s_mat4_set_rotation_z(_mat, _angle) { _mat.m[0][0] = cosf(_angle); _mat.m[0][1] = -sinf(_angle); _mat.m[1][0] = sinf(_angle); _mat.m[1][1] = cosf(_angle); }
#define s_mat4_get_rotation_z(_mat)      (atan2f(_mat.m[1][0], _mat.m[0][0]))
#define s_mat4_scale(_mat, _v)           (s_mat4_mul(_mat, &(s_mat4){ { {_v.x, 0.0f, 0.0f, 0.0f}, {0.0f, _v.y, 0.0f, 0.0f}, {0.0f, 0.0f, _v.z, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } }))
#define s_mat4_set_scale(_mat, _v)       { _mat.m[0][0] = _v.x; _mat.m[1][1] = _v.y; _mat.m[2][2] = _v.z; }
#define s_mat4_get_scale(_mat)           (s_vec3(_mat.m[0][0], _mat.m[1][1], _mat.m[2][2]))

#endif // SYPHAX_MATH
#endif // SYPHAX_TYPES

#define SYPHAX_ASSERT
#ifdef SYPHAX_ASSERT
#define s_assert(expr) if (!(expr))         { fprintf(stderr, "[%s: %d] Assertion failed: %s\n", __FILE__, __LINE__, #expr); assert(0); }
#define s_assertf(expr, ...) if (!(expr))   { fprintf(stderr, "[%s: %d] Assertion failed: %s\n", __FILE__, __LINE__, #expr); fprintf(stderr, __VA_ARGS__); assert(0); }
#endif // SYPHAX_ASSERT

#endif // S_TYPES_H
