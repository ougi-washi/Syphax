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

#ifndef SYPHAX_PRECISION
#define SYPHAX_PRECISION f32
#endif

typedef struct { SYPHAX_PRECISION x, y; } s_vec2;
typedef struct { SYPHAX_PRECISION x, y, z; } s_vec3;
typedef struct { SYPHAX_PRECISION x, y, z, w; } s_vec4;
typedef struct { SYPHAX_PRECISION m[3][3]; } s_mat3;
typedef struct { SYPHAX_PRECISION m[4][4]; } s_mat4;

#define s_vec(_vec_size, ...) (s_vec##_vec_size){ __VA_ARGS__ }
#define s_min(_a_vec, _b_vec) ((_a_vec) < (_b_vec) ? (_a_vec) : (_b_vec))
#define s_max(_a_vec, _b_vec) ((_a_vec) > (_b_vec) ? (_a_vec) : (_b_vec))
#define PI 3.14159265359

// s_vec2
#define s_vec2(_x, _y)                  (s_vec(2, (_x), (_y)))
#define s_vec2_add(_vec_a, _vec_b)      (s_vec2((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y))
#define s_vec2_sub(_vec_a, _vec_b)      (s_vec2((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y))
#define s_vec2_mul(_vec_a, _vec_b)      (s_vec2((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y))
#define s_vec2_div(_vec_a, _vec_b)      (s_vec2((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y))
#define s_vec2_muls(_vec, _s)           (s_vec2((_vec)->x * (_s), (_vec)->y * (_s)))
#define s_vec2_divs(_vec, _s)           (s_vec2((_vec)->x / (_s), (_vec)->y / (_s)))
#define s_vec2_dot(_vec_a, _vec_b)      ((_vec_a)->x * (_vec_b)->x + (_vec_a)->y * (_vec_b)->y)
#define s_vec2_cross(_vec_a, _vec_b)    ((_vec_a)->x * (_vec_b)->y - (_vec_a)->y * (_vec_b)->x)
#define s_vec2_length(_vec)             (sqrtf((_vec)->x * (_vec)->x + (_vec)->y * (_vec)->y))
#define s_vec2_lerp(_vec_a, _vec_b, _t)  (s_vec2((_vec_a)->x + (_t) * ((_vec_b)->x - (_vec_a)->x), (_vec_a)->y + (_t) * ((_vec_b)->y - (_vec_a)->y)))
#define s_vec2_from_angle(_vec)         (s_vec2(cosf(_vec), sinf(_vec)))
#define s_vec2_to_angle(_vec)           (atan2f((_vec)->y, (_vec)->x))
static inline s_vec2 s_vec2_normalize(const s_vec2* _vec) {
    const SYPHAX_PRECISION len = s_vec2_length(_vec);
    if (len == 0.0f) return s_vec2(0.0f, 0.0f);
    return s_vec2((_vec)->x / len, (_vec)->y / len);
}
static inline s_vec2 s_vec2_reflect(const s_vec2* _vec, const s_vec2* _n) {
    const SYPHAX_PRECISION dot = s_vec2_dot(_vec, _n);
    return s_vec2(_vec->x - 2.0f * dot * _n->x, _vec->y - 2.0f * dot * _n->y);
}

// s_vec3
#define s_vec3(_x, _y, _z)              (s_vec(3, (_x), (_y), (_z)))
#define s_vec3_add(_vec_a, _vec_b)      (s_vec3((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y, (_vec_a)->z + (_vec_b)->z))
#define s_vec3_sub(_vec_a, _vec_b)      (s_vec3((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y, (_vec_a)->z - (_vec_b)->z))
#define s_vec3_mul(_vec_a, _vec_b)      (s_vec3((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y, (_vec_a)->z * (_vec_b)->z))
#define s_vec3_div(_vec_a, _vec_b)      (s_vec3((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y, (_vec_a)->z / (_vec_b)->z))
#define s_vec3_muls(_vec, _s)           (s_vec3((_vec)->x * (_s), (_vec)->y * (_s), (_vec)->z * (_s)))
#define s_vec3_divs(_vec, _s)           (s_vec3((_vec)->x / (_s), (_vec)->y / (_s), (_vec)->z / (_s)))
#define s_vec3_dot(_vec_a, _vec_b)      ((_vec_a)->x * (_vec_b)->x + (_vec_a)->y * (_vec_b)->y + (_vec_a)->z * (_vec_b)->z)
#define s_vec3_cross(_vec_a, _vec_b)    (s_vec3((_vec_a)->y * (_vec_b)->z - (_vec_a)->z * (_vec_b)->y, (_vec_a)->z * (_vec_b)->x - (_vec_a)->x * (_vec_b)->z, (_vec_a)->x * (_vec_b)->y - (_vec_a)->y * (_vec_b)->x))
#define s_vec3_length(_vec)             (sqrtf((_vec)->x * (_vec)->x + (_vec)->y * (_vec)->y + (_vec)->z * (_vec)->z))
#define s_vec3_lerp(_vec_a, _vec_b, _t) (s_vec3((_vec_a)->x + (_t) * ((_vec_b)->x - (_vec_a)->x), (_vec_a)->y + (_t) * ((_vec_b)->y - (_vec_a)->y), (_vec_a)->z + (_t) * ((_vec_b)->z - (_vec_a)->z)))
#define s_vec3_from_angle(_vec)       (s_vec3(cosf(_vec), sinf(_vec), 0.0f))
#define s_vec3_to_angle(_vec)         (atan2f((_vec)->y, (_vec)->x))
static inline s_vec3 s_vec3_normalize(const s_vec3* _vec) {
    const SYPHAX_PRECISION len = s_vec3_length(_vec);
    if (len == 0.0f) return s_vec3(0.0f, 0.0f, 0.0f);
    return s_vec3((_vec)->x / len, (_vec)->y / len, (_vec)->z / len);
}
static inline s_vec3 s_vec3_reflect(const s_vec3* _vec, const s_vec3* _n) {
    const SYPHAX_PRECISION dot = s_vec3_dot(_vec, _n);
    return s_vec3(_vec->x - 2.0f * dot * _n->x, _vec->y - 2.0f * dot * _n->y, _vec->z - 2.0f * dot * _n->z);
}

// s_vec4
#define s_vec4(_x, _y, _z, _w)          (s_vec(4, (_x), (_y), (_z), (_w)))
#define s_vec4_add(_vec_a, _vec_b)      (s_vec4((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y, (_vec_a)->z + (_vec_b)->z, (_vec_a)->w + (_vec_b)->w))
#define s_vec4_sub(_vec_a, _vec_b)      (s_vec4((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y, (_vec_a)->z - (_vec_b)->z, (_vec_a)->w - (_vec_b)->w))
#define s_vec4_mul(_vec_a, _vec_b)      (s_vec4((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y, (_vec_a)->z * (_vec_b)->z, (_vec_a)->w * (_vec_b)->w))
#define s_vec4_div(_vec_a, _vec_b)      (s_vec4((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y, (_vec_a)->z / (_vec_b)->z, (_vec_a)->w / (_vec_b)->w))
#define s_vec4_muls(_vec, _s)           (s_vec4((_vec)->x * (_s), (_vec)->y * (_s), (_vec)->z * (_s), (_vec)->w * (_s)))
#define s_vec4_divs(_vec, _s)           (s_vec4((_vec)->x / (_s), (_vec)->y / (_s), (_vec)->z / (_s), (_vec)->w / (_s)))
#define s_vec4_dot(_vec_a, _vec_b)      ((_vec_a)->x * (_vec_b)->x + (_vec_a)->y * (_vec_b)->y + (_vec_a)->z * (_vec_b)->z + (_vec_a)->w * (_vec_b)->w)
#define s_vec4_cross(_vec_a, _vec_b)    (s_vec4((_vec_a)->y * (_vec_b)->z - (_vec_a)->z * (_vec_b)->y, (_vec_a)->z * (_vec_b)->x - (_vec_a)->x * (_vec_b)->z, (_vec_a)->x * (_vec_b)->y - (_vec_a)->y * (_vec_b)->x, 0.0f))
#define s_vec4_length(_vec)             (sqrtf((_vec)->x * (_vec)->x + (_vec)->y * (_vec)->y + (_vec)->z * (_vec)->z + (_vec)->w * (_vec)->w))
#define s_vec4_lerp(_vec_a, _vec_b, _t) (s_vec4((_vec_a)->x + (_t) * ((_vec_b)->x - (_vec_a)->x), (_vec_a)->y + (_t) * ((_vec_b)->y - (_vec_a)->y), (_vec_a)->z + (_t) * ((_vec_b)->z - (_vec_a)->z), (_vec_a)->w + (_t) * ((_vec_b)->w - (_vec_a)->w)))
#define s_vec4_from_angle(_vec)         (s_vec4(cosf(_vec), sinf(_vec), 0.0f, 0.0f))
#define s_vec4_to_angle(_vec)           (atan2f((_vec)->y, (_vec)->x))
static inline s_vec4 s_vec4_normalize(const s_vec4* _vec) {
    const SYPHAX_PRECISION len = s_vec4_length(_vec);
    if (len == 0.0f) return s_vec4(0.0f, 0.0f, 0.0f, 0.0f);
    return s_vec4((_vec)->x / len, (_vec)->y / len, (_vec)->z / len, (_vec)->w / len);
}
static inline s_vec4 s_vec4_reflect(const s_vec4* _vec, const s_vec4* _n) {
    const SYPHAX_PRECISION dot = s_vec4_dot(_vec, _n);
    return s_vec4(_vec->x - 2.0f * dot * _n->x, _vec->y - 2.0f * dot * _n->y, _vec->z - 2.0f * dot * _n->z, _vec->w - 2.0f * dot * _n->w);
}

// s_mat3
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
    for (u8 i = 0; i < 3; i++) {
        for (u8 j = 0; j < 3; j++) {
            result.m[i][j] = 0.0f;
            for (u8 k = 0; k < 3; k++) {
                result.m[i][j] += _mat_a->m[i][k] * _mat_b->m[k][j];
            }
        }
    }
    return result;
}
#define s_mat3_transpose(_mat) (s_mat3( \
	(_mat)->m[0][0], (_mat)->m[1][0], (_mat)->m[2][0], \
	(_mat)->m[0][1], (_mat)->m[1][1], (_mat)->m[2][1], \
	(_mat)->m[0][2], (_mat)->m[1][2], (_mat)->m[2][2] \
))
static inline s_mat3 s_mat3_inverse(const s_mat3* _mat) {
    s_mat3 inv = *_mat;
    SYPHAX_PRECISION aug[3][6];
    for (u8 i = 0; i < 3; i++) {
        for (u8 j = 0; j < 3; j++) aug[i][j] = inv.m[i][j];
        for (u8 j = 0; j < 3; j++) aug[i][j + 3] = (i == j) ? 1.0f : 0.0f;
    }
    for (u8 i = 0; i < 3; i++) {
        f32 pivot = aug[i][i];
        if (pivot == 0.0f) return s_mat3_identity;
        for (u8 j = 0; j < 6; j++) aug[i][j] /= pivot;
        for (u8 k = 0; k < 3; k++) {
            if (k != i) {
                f32 factor = aug[k][i];
                for (u8 j = 0; j < 6; j++) aug[k][j] -= factor * aug[i][j];
            }
        }
    }
    for (u8 i = 0; i < 3; i++)
        for (u8 j = 0; j < 3; j++)
            inv.m[i][j] = aug[i][j + 3];
    return inv;
}
#define s_mat3_translate(_mat, _v)        (s_mat3_mul((_mat), &(s_mat3){ { {1.0f, 0.0f, (_v)->x}, {0.0f, 1.0f, (_v)->y}, {0.0f, 0.0f, 1.0f} } }))
#define s_mat3_set_translation(_mat, _v)  { (_mat)->m[0][2] = (_v)->x; (_mat)->m[1][2] = (_v)->y; }
#define s_mat3_get_translation(_mat)      (s_vec2((_mat)->m[0][2], (_mat)->m[1][2]))
#define s_mat3_rotate(_mat, _angle)       (s_mat3_mul((_mat), &(s_mat3){ { {cosf(_angle), -sinf(_angle), 0.0f}, {sinf(_angle), cosf(_angle), 0.0f}, {0.0f, 0.0f, 1.0f} } }))
#define s_mat3_set_rotation(_mat, _angle) { (_mat)->m[0][0] = cosf(_angle); (_mat)->m[0][1] = -sinf(_angle); (_mat)->m[1][0] = sinf(_angle); (_mat)->m[1][1] = cosf(_angle); }
#define s_mat3_get_rotation(_mat)         (atan2f((_mat)->m[1][0], (_mat)->m[0][0]))
#define s_mat3_scale(_mat, _v)            (s_mat3_mul((_mat), &(s_mat3){ { {(_v)->x, 0.0f, 0.0f}, {0.0f, (_v)->y, 0.0f}, {0.0f, 0.0f, 1.0f} } }))
#define s_mat3_set_scale(_mat, _v)        { (_mat)->m[0][0] = (_v)->x; (_mat)->m[1][1] = (_v)->y; }
#define s_mat3_get_scale(_mat)            (s_vec2((_mat)->m[0][0], (_mat)->m[1][1]))
#define s_mat3_shear(_mat, _v)            (s_mat3_mul((_mat), &(s_mat3){ { {1.0f, (_v)->x, 0.0f}, {(_v)->y, 1.0f, 0.0f}, {0.0f, 0.0f, 1.0f} } }))
#define s_mat3_set_shear(_mat, _v)        { (_mat)->m[0][1] = (_v)->x; (_mat)->m[1][0] = (_v)->y; }
#define s_mat3_get_shear(_mat)            (s_vec2((_mat)->m[0][1], (_mat)->m[1][0]))
static inline s_mat3 s_mat3_look_at(const s_vec3* _from, const s_vec3* _to, const s_vec3* _up) {
    s_vec3 f = s_vec3_sub(_to, _from);
    f.z = 0.0f;
    f = s_vec3_normalize(&f);
    s_vec3 s = s_vec3_cross(&f, _up);
    s = s_vec3_normalize(&s);
    s_vec3 u = s_vec3_cross(&s, &f);
    s_mat3 result = s_mat3_identity;
    result.m[0][0] = s.x; result.m[0][1] = u.x; result.m[0][2] = -s_vec3_dot(&s, _from);
    result.m[1][0] = s.y; result.m[1][1] = u.y; result.m[1][2] = -s_vec3_dot(&u, _from);
    result.m[2][0] = 0.0f; result.m[2][1] = 0.0f; result.m[2][2] = 1.0f;
    return result;
}

// s_mat4
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
    for (u8 i = 0; i < 4; i++) {
        for (u8 j = 0; j < 4; j++) {
            result.m[i][j] = 0.0f;
            for (u8 k = 0; k < 4; k++) {
                result.m[i][j] += _mat_a->m[i][k] * _mat_b->m[k][j];
            }
        }
    }
    return result;
}
#define s_mat4_transpose(_mat) (s_mat4( \
	(_mat)->m[0][0], (_mat)->m[1][0], (_mat)->m[2][0], (_mat)->m[3][0], \
	(_mat)->m[0][1], (_mat)->m[1][1], (_mat)->m[2][1], (_mat)->m[3][1], \
	(_mat)->m[0][2], (_mat)->m[1][2], (_mat)->m[2][2], (_mat)->m[3][2], \
	(_mat)->m[0][3], (_mat)->m[1][3], (_mat)->m[2][3], (_mat)->m[3][3] \
))
static inline s_mat4 s_mat4_inverse(const s_mat4* _mat) {
    s_mat4 inv = *_mat;
    SYPHAX_PRECISION aug[4][8];
    for (u8 i = 0; i < 4; i++) {
        for (u8 j = 0; j < 4; j++) aug[i][j] = inv.m[i][j];
        for (u8 j = 0; j < 4; j++) aug[i][j + 4] = (i == j) ? 1.0f : 0.0f;
    }
    for (u8 i = 0; i < 4; i++) {
        f32 pivot = aug[i][i];
        if (pivot == 0.0f) return s_mat4_identity;
        for (u8 j = 0; j < 8; j++) aug[i][j] /= pivot;
        for (u8 k = 0; k < 4; k++) {
            if (k != i) {
                f32 factor = aug[k][i];
                for (u8 j = 0; j < 8; j++) aug[k][j] -= factor * aug[i][j];
            }
        }
    }
    for (u8 i = 0; i < 4; i++)
        for (u8 j = 0; j < 4; j++)
            inv.m[i][j] = aug[i][j + 4];
    return inv;
}
#define s_mat4_translate(_mat, _v)          (s_mat4_mul((_mat), &(s_mat4){ { {1.0f, 0.0f, 0.0f, (_v)->x}, {0.0f, 1.0f, 0.0f, (_v)->y}, {0.0f, 0.0f, 1.0f, (_v)->z}, {0.0f, 0.0f, 0.0f, 1.0f} } }))
#define s_mat4_set_translation(_mat, _v)    { (_mat)->m[0][3] = (_v)->x; (_mat)->m[1][3] = (_v)->y; (_mat)->m[2][3] = (_v)->z; }
#define s_mat4_get_translation(_mat)        (s_vec3((_mat)->m[0][3], (_mat)->m[1][3], (_mat)->m[2][3]))
#define s_mat4_rotate_x(_mat, _angle)       (s_mat4_mul((_mat), &(s_mat4){ { {1.0f, 0.0f, 0.0f, 0.0f}, {0.0f, cosf(_angle), -sinf(_angle), 0.0f}, {0.0f, sinf(_angle), cosf(_angle), 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } }))
#define s_mat4_set_rotation_x(_mat, _angle) { (_mat)->m[1][1] = cosf(_angle); (_mat)->m[1][2] = -sinf(_angle); (_mat)->m[2][1] = sinf(_angle); (_mat)->m[2][2] = cosf(_angle); }
#define s_mat4_get_rotation_x(_mat)         (atan2f((_mat)->m[2][1], (_mat)->m[1][1]))
#define s_mat4_rotate_y(_mat, _angle)       (s_mat4_mul((_mat), &(s_mat4){ { {cosf(_angle), 0.0f, sinf(_angle), 0.0f}, {0.0f, 1.0f, 0.0f, 0.0f}, {-sinf(_angle), 0.0f, cosf(_angle), 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } }))
#define s_mat4_set_rotation_y(_mat, _angle) { (_mat)->m[0][0] = cosf(_angle); (_mat)->m[0][2] = -sinf(_angle); (_mat)->m[2][0] = sinf(_angle); (_mat)->m[2][2] = cosf(_angle); }                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                         
#define s_mat4_get_rotation_y(_mat)         (atan2f((_mat)->m[2][0], (_mat)->m[0][0]))
#define s_mat4_rotate_z(_mat, _angle)       (s_mat4_mul((_mat), &(s_mat4){ { {cosf(_angle), -sinf(_angle), 0.0f, 0.0f}, {sinf(_angle), cosf(_angle), 0.0f, 0.0f}, {0.0f, 0.0f, 1.0f, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } }))
#define s_mat4_set_rotation_z(_mat, _angle) { (_mat)->m[0][0] = cosf(_angle); (_mat)->m[0][1] = -sinf(_angle); (_mat)->m[1][0] = sinf(_angle); (_mat)->m[1][1] = cosf(_angle); }
#define s_mat4_get_rotation_z(_mat)         (atan2f((_mat)->m[1][0], (_mat)->m[0][0]))
#define s_mat4_scale(_mat, _v)              (s_mat4_mul((_mat), &(s_mat4){ { {(_v)->x, 0.0f, 0.0f, 0.0f}, {0.0f, (_v)->y, 0.0f, 0.0f}, {0.0f, 0.0f, (_v)->z, 0.0f}, {0.0f, 0.0f, 0.0f, 1.0f} } }))
#define s_mat4_set_scale(_mat, _v)          { (_mat)->m[0][0] = (_v)->x; (_mat)->m[1][1] = (_v)->y; (_mat)->m[2][2] = (_v)->z; }
#define s_mat4_get_scale(_mat)              (s_vec3((_mat)->m[0][0], (_mat)->m[1][1], (_mat)->m[2][2]))
static inline s_mat4 s_mat4_look_at(const s_vec3* _from, const s_vec3* _to, const s_vec3* _up) {
    s_vec3 f = s_vec3_sub(_to, _from);
    f = s_vec3_normalize(&f);
    s_vec3 s = s_vec3_cross(&f, _up);
    s = s_vec3_normalize(&s);
    s_vec3 u = s_vec3_cross(&s, &f);
    s_mat4 result = s_mat4_identity;
    result.m[0][0] = s.x; result.m[0][1] = u.x; result.m[0][2] = -f.x; result.m[0][3] = -s_vec3_dot(&s, _from);
    result.m[1][0] = s.y; result.m[1][1] = u.y; result.m[1][2] = -f.y; result.m[1][3] = -s_vec3_dot(&s, _from);
    result.m[2][0] = s.z; result.m[2][1] = u.z; result.m[2][2] = -f.z; result.m[2][3] = -s_vec3_dot(&s, _from);
    return result;
}
static inline s_mat4 s_mat4_perspective(f32 _fov, f32 _aspect, f32 _near, f32 _far) {
    f32 tanHalfFov = tanf(_fov / 2.0f);
    s_mat4 result = {0};
    result.m[0][0] = 1.0f / (_aspect * tanHalfFov);
    result.m[1][1] = 1.0f / tanHalfFov;
    result.m[2][2] = -(_far + _near) / (_far - _near);
    result.m[2][3] = -(2.0f * _far * _near) / (_far - _near);
    result.m[3][2] = -1.0f;
    return result;
}

#define s_assert(expr) if (!(expr))         { fprintf(stderr, "[%s: %d] Assertion failed: %s\n", __FILE__, __LINE__, #expr); assert(0); }
#define s_assertf(expr, ...) if (!(expr))   { fprintf(stderr, "[%s: %d] Assertion failed: %s\n", __FILE__, __LINE__, #expr); fprintf(stderr, __VA_ARGS__); assert(0); }

#endif // S_TYPES_H

