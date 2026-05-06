/**
 * Syphax - Ougi Washi
 *
 * Generic math helpers
 *
 * MIT License
 * Copyright (c) 2025-2026 Jed - Ougi Washi https://github.com/ougi-washi/Syphax
 */

#ifndef S_MATH_H
#define S_MATH_H

#include "s_types.h"

#include <float.h>
#include <math.h>

#ifndef S_PRECISION
#define S_PRECISION f32
#endif

typedef S_PRECISION s_real;

typedef struct { s_real x, y; } s_vec2;
typedef struct { s_real x, y, z; } s_vec3;
typedef struct { s_real x, y, z, w; } s_vec4;
typedef struct { i32 x, y; } s_ivec2;
typedef struct { i32 x, y, z; } s_ivec3;
typedef struct { i32 x, y, z, w; } s_ivec4;
typedef struct { u32 x, y; } s_uvec2;
typedef struct { u32 x, y, z; } s_uvec3;
typedef struct { u32 x, y, z, w; } s_uvec4;
typedef struct { s_real m[3][3]; } s_mat3;
typedef struct { s_real m[4][4]; } s_mat4;

#define s_vec(_vec_size, ...) (s_vec##_vec_size){ __VA_ARGS__ }

static inline s_real s_real_abs(s_real _v) { return (s_real)fabs((double)_v); }
static inline s_real s_real_sqrt(s_real _v) { return (s_real)sqrt((double)_v); }
static inline s_real s_real_sin(s_real _v) { return (s_real)sin((double)_v); }
static inline s_real s_real_cos(s_real _v) { return (s_real)cos((double)_v); }
static inline s_real s_real_tan(s_real _v) { return (s_real)tan((double)_v); }
static inline s_real s_real_atan2(s_real _y, s_real _x) { return (s_real)atan2((double)_y, (double)_x); }
static inline s_real s_real_min(s_real _a, s_real _b) { return _a < _b ? _a : _b; }
static inline s_real s_real_max(s_real _a, s_real _b) { return _a > _b ? _a : _b; }
static inline i32 s_i32_abs(i32 _v) { return _v == INT32_MIN ? INT32_MAX : (_v < 0 ? -_v : _v); }

#define s_min(_a_vec, _b_vec) ((_a_vec) < (_b_vec) ? (_a_vec) : (_b_vec))
#define s_max(_a_vec, _b_vec) ((_a_vec) > (_b_vec) ? (_a_vec) : (_b_vec))
#define PI 3.14159265358979323846

#ifndef S_EPSILON
#define S_EPSILON ((s_real)1e-6)
#endif

static inline b8 s_f32_equal(const f32 _a, const f32 _b, const f32 _eps) {
    return fabsf(_a - _b) <= fabsf(_eps);
}

static inline b8 s_f64_equal(const f64 _a, const f64 _b, const f64 _eps) {
    return fabs(_a - _b) <= fabs(_eps);
}

static inline b8 s_precision_equal(const s_real _a, const s_real _b, const s_real _eps) {
    return s_real_abs(_a - _b) <= s_real_abs(_eps);
}

static inline b8 s_vec2_equal(const s_vec2* _a, const s_vec2* _b, const s_real _eps) {
    if (_a == NULL || _b == NULL) return false;
    return s_precision_equal(_a->x, _b->x, _eps)
        && s_precision_equal(_a->y, _b->y, _eps);
}

static inline b8 s_vec3_equal(const s_vec3* _a, const s_vec3* _b, const s_real _eps) {
    if (_a == NULL || _b == NULL) return false;
    return s_precision_equal(_a->x, _b->x, _eps)
        && s_precision_equal(_a->y, _b->y, _eps)
        && s_precision_equal(_a->z, _b->z, _eps);
}

static inline b8 s_vec4_equal(const s_vec4* _a, const s_vec4* _b, const s_real _eps) {
    if (_a == NULL || _b == NULL) return false;
    return s_precision_equal(_a->x, _b->x, _eps)
        && s_precision_equal(_a->y, _b->y, _eps)
        && s_precision_equal(_a->z, _b->z, _eps)
        && s_precision_equal(_a->w, _b->w, _eps);
}

static inline b8 s_mat3_equal(const s_mat3* _a, const s_mat3* _b, const s_real _eps) {
    if (_a == NULL || _b == NULL) return false;
    for (u8 row = 0; row < 3; row++) {
        for (u8 col = 0; col < 3; col++) {
            if (!s_precision_equal(_a->m[row][col], _b->m[row][col], _eps)) return false;
        }
    }
    return true;
}

static inline b8 s_mat4_equal(const s_mat4* _a, const s_mat4* _b, const s_real _eps) {
    if (_a == NULL || _b == NULL) return false;
    for (u8 col = 0; col < 4; col++) {
        for (u8 row = 0; row < 4; row++) {
            if (!s_precision_equal(_a->m[col][row], _b->m[col][row], _eps)) return false;
        }
    }
    return true;
}

// s_vec2
#define s_vec2(_x, _y)                  (s_vec(2, (_x), (_y)))
#define s_vec2_abs(_vec)                (s_vec2(s_real_abs((_vec)->x), s_real_abs((_vec)->y)))
#define s_vec2_add(_vec_a, _vec_b)      (s_vec2((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y))
#define s_vec2_sub(_vec_a, _vec_b)      (s_vec2((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y))
#define s_vec2_mul(_vec_a, _vec_b)      (s_vec2((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y))
#define s_vec2_div(_vec_a, _vec_b)      (s_vec2((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y))
#define s_vec2_muls(_vec, _s)           (s_vec2((_vec)->x * (_s), (_vec)->y * (_s)))
#define s_vec2_divs(_vec, _s)           (s_vec2((_vec)->x / (_s), (_vec)->y / (_s)))
#define s_vec2_dot(_vec_a, _vec_b)      ((_vec_a)->x * (_vec_b)->x + (_vec_a)->y * (_vec_b)->y)
#define s_vec2_cross(_vec_a, _vec_b)    ((_vec_a)->x * (_vec_b)->y - (_vec_a)->y * (_vec_b)->x)
#define s_vec2_length(_vec)             (s_real_sqrt((_vec)->x * (_vec)->x + (_vec)->y * (_vec)->y))
#define s_vec2_lerp(_vec_a, _vec_b, _t)  (s_vec2((_vec_a)->x + (_t) * ((_vec_b)->x - (_vec_a)->x), (_vec_a)->y + (_t) * ((_vec_b)->y - (_vec_a)->y)))
#define s_vec2_from_angle(_vec)         (s_vec2(s_real_cos(_vec), s_real_sin(_vec)))
#define s_vec2_to_angle(_vec)           (s_real_atan2((_vec)->y, (_vec)->x))
static inline s_vec2 s_vec2_normalize(const s_vec2* _vec) {
    const s_real len = s_vec2_length(_vec);
    if (len == 0.0f) return s_vec2(0.0f, 0.0f);
    return s_vec2((_vec)->x / len, (_vec)->y / len);
}
static inline s_vec2 s_vec2_reflect(const s_vec2* _vec, const s_vec2* _n) {
    const s_real dot = s_vec2_dot(_vec, _n);
    return s_vec2(_vec->x - 2.0f * dot * _n->x, _vec->y - 2.0f * dot * _n->y);
}

// s_vec3
#define s_vec3(_x, _y, _z)              (s_vec(3, (_x), (_y), (_z)))
#define s_vec3_abs(_vec)                (s_vec3(s_real_abs((_vec)->x), s_real_abs((_vec)->y), s_real_abs((_vec)->z)))
#define s_vec3_add(_vec_a, _vec_b)      (s_vec3((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y, (_vec_a)->z + (_vec_b)->z))
#define s_vec3_sub(_vec_a, _vec_b)      (s_vec3((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y, (_vec_a)->z - (_vec_b)->z))
#define s_vec3_mul(_vec_a, _vec_b)      (s_vec3((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y, (_vec_a)->z * (_vec_b)->z))
#define s_vec3_div(_vec_a, _vec_b)      (s_vec3((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y, (_vec_a)->z / (_vec_b)->z))
#define s_vec3_muls(_vec, _s)           (s_vec3((_vec)->x * (_s), (_vec)->y * (_s), (_vec)->z * (_s)))
#define s_vec3_divs(_vec, _s)           (s_vec3((_vec)->x / (_s), (_vec)->y / (_s), (_vec)->z / (_s)))
#define s_vec3_dot(_vec_a, _vec_b)      ((_vec_a)->x * (_vec_b)->x + (_vec_a)->y * (_vec_b)->y + (_vec_a)->z * (_vec_b)->z)
#define s_vec3_cross(_vec_a, _vec_b)    (s_vec3((_vec_a)->y * (_vec_b)->z - (_vec_a)->z * (_vec_b)->y, (_vec_a)->z * (_vec_b)->x - (_vec_a)->x * (_vec_b)->z, (_vec_a)->x * (_vec_b)->y - (_vec_a)->y * (_vec_b)->x))
#define s_vec3_length(_vec)             (s_real_sqrt((_vec)->x * (_vec)->x + (_vec)->y * (_vec)->y + (_vec)->z * (_vec)->z))
#define s_vec3_lerp(_vec_a, _vec_b, _t) (s_vec3((_vec_a)->x + (_t) * ((_vec_b)->x - (_vec_a)->x), (_vec_a)->y + (_t) * ((_vec_b)->y - (_vec_a)->y), (_vec_a)->z + (_t) * ((_vec_b)->z - (_vec_a)->z)))
#define s_vec3_from_angle(_vec)       (s_vec3(s_real_cos(_vec), s_real_sin(_vec), 0.0f))
#define s_vec3_to_angle(_vec)         (s_real_atan2((_vec)->y, (_vec)->x))
static inline s_vec3 s_vec3_normalize(const s_vec3* _vec) {
    const s_real len = s_vec3_length(_vec);
    if (len == 0.0f) return s_vec3(0.0f, 0.0f, 0.0f);
    return s_vec3((_vec)->x / len, (_vec)->y / len, (_vec)->z / len);
}
static inline s_vec3 s_vec3_reflect(const s_vec3* _vec, const s_vec3* _n) {
    const s_real dot = s_vec3_dot(_vec, _n);
    return s_vec3(_vec->x - 2.0f * dot * _n->x, _vec->y - 2.0f * dot * _n->y, _vec->z - 2.0f * dot * _n->z);
}

// s_vec4
#define s_vec4(_x, _y, _z, _w)          (s_vec(4, (_x), (_y), (_z), (_w)))
#define s_vec4_abs(_vec)                (s_vec4(s_real_abs((_vec)->x), s_real_abs((_vec)->y), s_real_abs((_vec)->z), s_real_abs((_vec)->w)))
#define s_vec4_add(_vec_a, _vec_b)      (s_vec4((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y, (_vec_a)->z + (_vec_b)->z, (_vec_a)->w + (_vec_b)->w))
#define s_vec4_sub(_vec_a, _vec_b)      (s_vec4((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y, (_vec_a)->z - (_vec_b)->z, (_vec_a)->w - (_vec_b)->w))
#define s_vec4_mul(_vec_a, _vec_b)      (s_vec4((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y, (_vec_a)->z * (_vec_b)->z, (_vec_a)->w * (_vec_b)->w))
#define s_vec4_div(_vec_a, _vec_b)      (s_vec4((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y, (_vec_a)->z / (_vec_b)->z, (_vec_a)->w / (_vec_b)->w))
#define s_vec4_muls(_vec, _s)           (s_vec4((_vec)->x * (_s), (_vec)->y * (_s), (_vec)->z * (_s), (_vec)->w * (_s)))
#define s_vec4_divs(_vec, _s)           (s_vec4((_vec)->x / (_s), (_vec)->y / (_s), (_vec)->z / (_s), (_vec)->w / (_s)))
#define s_vec4_dot(_vec_a, _vec_b)      ((_vec_a)->x * (_vec_b)->x + (_vec_a)->y * (_vec_b)->y + (_vec_a)->z * (_vec_b)->z + (_vec_a)->w * (_vec_b)->w)
#define s_vec4_cross(_vec_a, _vec_b)    (s_vec4((_vec_a)->y * (_vec_b)->z - (_vec_a)->z * (_vec_b)->y, (_vec_a)->z * (_vec_b)->x - (_vec_a)->x * (_vec_b)->z, (_vec_a)->x * (_vec_b)->y - (_vec_a)->y * (_vec_b)->x, 0.0f))
#define s_vec4_length(_vec)             (s_real_sqrt((_vec)->x * (_vec)->x + (_vec)->y * (_vec)->y + (_vec)->z * (_vec)->z + (_vec)->w * (_vec)->w))
#define s_vec4_lerp(_vec_a, _vec_b, _t) (s_vec4((_vec_a)->x + (_t) * ((_vec_b)->x - (_vec_a)->x), (_vec_a)->y + (_t) * ((_vec_b)->y - (_vec_a)->y), (_vec_a)->z + (_t) * ((_vec_b)->z - (_vec_a)->z), (_vec_a)->w + (_t) * ((_vec_b)->w - (_vec_a)->w)))
#define s_vec4_from_angle(_vec)         (s_vec4(s_real_cos(_vec), s_real_sin(_vec), 0.0f, 0.0f))
#define s_vec4_to_angle(_vec)           (s_real_atan2((_vec)->y, (_vec)->x))
static inline s_vec4 s_vec4_normalize(const s_vec4* _vec) {
    const s_real len = s_vec4_length(_vec);
    if (len == 0.0f) return s_vec4(0.0f, 0.0f, 0.0f, 0.0f);
    return s_vec4((_vec)->x / len, (_vec)->y / len, (_vec)->z / len, (_vec)->w / len);
}
static inline s_vec4 s_vec4_reflect(const s_vec4* _vec, const s_vec4* _n) {
    const s_real dot = s_vec4_dot(_vec, _n);
    return s_vec4(_vec->x - 2.0f * dot * _n->x, _vec->y - 2.0f * dot * _n->y, _vec->z - 2.0f * dot * _n->z, _vec->w - 2.0f * dot * _n->w);
}

// s_ivec2
#define s_ivec2(_x, _y)                  ((s_ivec2){ (_x), (_y) })
#define s_ivec2_abs(_vec)                (s_ivec2(s_i32_abs((_vec)->x), s_i32_abs((_vec)->y)))
#define s_ivec2_add(_vec_a, _vec_b)      (s_ivec2((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y))
#define s_ivec2_sub(_vec_a, _vec_b)      (s_ivec2((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y))
#define s_ivec2_mul(_vec_a, _vec_b)      (s_ivec2((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y))
#define s_ivec2_div(_vec_a, _vec_b)      (s_ivec2((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y))

// s_ivec3
#define s_ivec3(_x, _y, _z)              ((s_ivec3){ (_x), (_y), (_z) })
#define s_ivec3_abs(_vec)                (s_ivec3(s_i32_abs((_vec)->x), s_i32_abs((_vec)->y), s_i32_abs((_vec)->z)))
#define s_ivec3_add(_vec_a, _vec_b)      (s_ivec3((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y, (_vec_a)->z + (_vec_b)->z))
#define s_ivec3_sub(_vec_a, _vec_b)      (s_ivec3((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y, (_vec_a)->z - (_vec_b)->z))
#define s_ivec3_mul(_vec_a, _vec_b)      (s_ivec3((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y, (_vec_a)->z * (_vec_b)->z))
#define s_ivec3_div(_vec_a, _vec_b)      (s_ivec3((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y, (_vec_a)->z / (_vec_b)->z))

// s_ivec4
#define s_ivec4(_x, _y, _z, _w)          ((s_ivec4){ (_x), (_y), (_z), (_w) })
#define s_ivec4_abs(_vec)                (s_ivec4(s_i32_abs((_vec)->x), s_i32_abs((_vec)->y), s_i32_abs((_vec)->z), s_i32_abs((_vec)->w)))
#define s_ivec4_add(_vec_a, _vec_b)      (s_ivec4((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y, (_vec_a)->z + (_vec_b)->z, (_vec_a)->w + (_vec_b)->w))
#define s_ivec4_sub(_vec_a, _vec_b)      (s_ivec4((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y, (_vec_a)->z - (_vec_b)->z, (_vec_a)->w - (_vec_b)->w))
#define s_ivec4_mul(_vec_a, _vec_b)      (s_ivec4((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y, (_vec_a)->z * (_vec_b)->z, (_vec_a)->w * (_vec_b)->w))
#define s_ivec4_div(_vec_a, _vec_b)      (s_ivec4((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y, (_vec_a)->z / (_vec_b)->z, (_vec_a)->w / (_vec_b)->w))

// s_uvec2
#define s_uvec2(_x, _y)                  ((s_uvec2){ (_x), (_y) })
#define s_uvec2_add(_vec_a, _vec_b)      (s_uvec2((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y))
#define s_uvec2_sub(_vec_a, _vec_b)      (s_uvec2((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y))
#define s_uvec2_mul(_vec_a, _vec_b)      (s_uvec2((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y))
#define s_uvec2_div(_vec_a, _vec_b)      (s_uvec2((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y))

// s_uvec3
#define s_uvec3(_x, _y, _z)              ((s_uvec3){ (_x), (_y), (_z) })
#define s_uvec3_add(_vec_a, _vec_b)      (s_uvec3((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y, (_vec_a)->z + (_vec_b)->z))
#define s_uvec3_sub(_vec_a, _vec_b)      (s_uvec3((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y, (_vec_a)->z - (_vec_b)->z))
#define s_uvec3_mul(_vec_a, _vec_b)      (s_uvec3((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y, (_vec_a)->z * (_vec_b)->z))
#define s_uvec3_div(_vec_a, _vec_b)      (s_uvec3((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y, (_vec_a)->z / (_vec_b)->z))

// s_uvec4
#define s_uvec4(_x, _y, _z, _w)          ((s_uvec4){ (_x), (_y), (_z), (_w) })
#define s_uvec4_add(_vec_a, _vec_b)      (s_uvec4((_vec_a)->x + (_vec_b)->x, (_vec_a)->y + (_vec_b)->y, (_vec_a)->z + (_vec_b)->z, (_vec_a)->w + (_vec_b)->w))
#define s_uvec4_sub(_vec_a, _vec_b)      (s_uvec4((_vec_a)->x - (_vec_b)->x, (_vec_a)->y - (_vec_b)->y, (_vec_a)->z - (_vec_b)->z, (_vec_a)->w - (_vec_b)->w))
#define s_uvec4_mul(_vec_a, _vec_b)      (s_uvec4((_vec_a)->x * (_vec_b)->x, (_vec_a)->y * (_vec_b)->y, (_vec_a)->z * (_vec_b)->z, (_vec_a)->w * (_vec_b)->w))
#define s_uvec4_div(_vec_a, _vec_b)      (s_uvec4((_vec_a)->x / (_vec_b)->x, (_vec_a)->y / (_vec_b)->y, (_vec_a)->z / (_vec_b)->z, (_vec_a)->w / (_vec_b)->w))

// s_mat3
// Matrices use column-major storage: m[column][row]. Transform helpers assume
// column vectors, so translation lives in the last column.
#define s_mat3(_m00, _m01, _m02, _m10, _m11, _m12, _m20, _m21, _m22) \
	(s_mat3){ \
		.m = { \
			{ _m00, _m10, _m20 }, \
			{ _m01, _m11, _m21 }, \
			{ _m02, _m12, _m22 } \
		} \
	}
#define s_mat3_identity (s_mat3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f))
static inline s_mat3 s_mat3_mul(const s_mat3* _mat_a, const s_mat3* _mat_b) {
    s_mat3 result;
    for (u8 c = 0; c < 3; c++) {
        for (u8 r = 0; r < 3; r++) {
            result.m[c][r] = 0.0f;
            for (u8 k = 0; k < 3; k++) {
                result.m[c][r] += _mat_a->m[k][r] * _mat_b->m[c][k];
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
static inline b8 s_mat3_inverse_checked(const s_mat3* _mat, s_mat3* _out) {
    if (_mat == NULL || _out == NULL) return false;
    s_real aug[3][6];
    for (u8 i = 0; i < 3; i++) {
        for (u8 j = 0; j < 3; j++) aug[i][j] = _mat->m[j][i];
        for (u8 j = 0; j < 3; j++) aug[i][j + 3] = (i == j) ? 1.0f : 0.0f;
    }
    for (u8 i = 0; i < 3; i++) {
        u8 pivot_row = i;
        s_real pivot_abs = s_real_abs(aug[i][i]);
        for (u8 r = (u8)(i + 1); r < 3; ++r) {
            s_real value_abs = s_real_abs(aug[r][i]);
            if (value_abs > pivot_abs) {
                pivot_abs = value_abs;
                pivot_row = r;
            }
        }
        if (pivot_abs <= S_EPSILON) return false;
        if (pivot_row != i) {
            for (u8 j = 0; j < 6; ++j) {
                s_real tmp = aug[i][j];
                aug[i][j] = aug[pivot_row][j];
                aug[pivot_row][j] = tmp;
            }
        }
        s_real pivot = aug[i][i];
        for (u8 j = 0; j < 6; j++) aug[i][j] /= pivot;
        for (u8 k = 0; k < 3; k++) {
            if (k != i) {
                s_real factor = aug[k][i];
                for (u8 j = 0; j < 6; j++) aug[k][j] -= factor * aug[i][j];
            }
        }
    }
    for (u8 i = 0; i < 3; i++)
        for (u8 j = 0; j < 3; j++)
            _out->m[j][i] = aug[i][j + 3];
    return true;
}
static inline s_mat3 s_mat3_inverse(const s_mat3* _mat) {
    s_mat3 inv = s_mat3_identity;
    if (!s_mat3_inverse_checked(_mat, &inv)) return s_mat3_identity;
    return inv;
}
#define s_mat3_translate(_mat, _v)        (s_mat3_mul((_mat), &(s_mat3(1.0f, 0.0f, (_v)->x, 0.0f, 1.0f, (_v)->y, 0.0f, 0.0f, 1.0f))))
#define s_mat3_set_translation(_mat, _v)  { (_mat)->m[2][0] = (_v)->x; (_mat)->m[2][1] = (_v)->y; }
#define s_mat3_get_translation(_mat)      (s_vec2((_mat)->m[2][0], (_mat)->m[2][1]))
#define s_mat3_rotate(_mat, _angle)       (s_mat3_mul((_mat), &(s_mat3(s_real_cos(_angle), -s_real_sin(_angle), 0.0f, s_real_sin(_angle), s_real_cos(_angle), 0.0f, 0.0f, 0.0f, 1.0f))))
#define s_mat3_set_rotation(_mat, _angle) { (_mat)->m[0][0] = s_real_cos(_angle); (_mat)->m[1][0] = -s_real_sin(_angle); (_mat)->m[0][1] = s_real_sin(_angle); (_mat)->m[1][1] = s_real_cos(_angle); }
#define s_mat3_get_rotation(_mat)         (s_real_atan2((_mat)->m[0][1], (_mat)->m[0][0]))
#define s_mat3_scale(_mat, _v)            (s_mat3_mul((_mat), &(s_mat3((_v)->x, 0.0f, 0.0f, 0.0f, (_v)->y, 0.0f, 0.0f, 0.0f, 1.0f))))
#define s_mat3_set_scale(_mat, _v)        { (_mat)->m[0][0] = (_v)->x; (_mat)->m[1][1] = (_v)->y; }
#define s_mat3_get_scale(_mat)            (s_vec2((_mat)->m[0][0], (_mat)->m[1][1]))
#define s_mat3_shear(_mat, _v)            (s_mat3_mul((_mat), &(s_mat3(1.0f, (_v)->y, 0.0f, (_v)->x, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f))))
#define s_mat3_set_shear(_mat, _v)        { (_mat)->m[0][1] = (_v)->y; (_mat)->m[1][0] = (_v)->x; }
#define s_mat3_get_shear(_mat)            (s_vec2((_mat)->m[1][0], (_mat)->m[0][1]))
static inline s_mat3 s_mat3_look_at(const s_vec3* _from, const s_vec3* _to, const s_vec3* _up) {
    if (_from == NULL || _to == NULL || _up == NULL) return s_mat3_identity;
    s_vec3 f = s_vec3_sub(_to, _from);
    f.z = 0.0f;
    f = s_vec3_normalize(&f);
    s_vec3 s = s_vec3_cross(&f, _up);
    s = s_vec3_normalize(&s);
    s_vec3 u = s_vec3_cross(&s, &f);
    s_mat3 result = s_mat3_identity;
    result.m[0][0] = s.x; result.m[0][1] = s.y; result.m[0][2] = 0.0f;
    result.m[1][0] = u.x; result.m[1][1] = u.y; result.m[1][2] = 0.0f;
    result.m[2][0] = -s_vec3_dot(&s, _from); result.m[2][1] = -s_vec3_dot(&u, _from); result.m[2][2] = 1.0f;
    return result;
}

// s_mat4
#define s_mat4(_m00, _m01, _m02, _m03, _m10, _m11, _m12, _m13, _m20, _m21, _m22, _m23, _m30, _m31, _m32, _m33) \
	(s_mat4){ \
		.m = { \
			{ _m00, _m10, _m20, _m30 }, \
			{ _m01, _m11, _m21, _m31 }, \
			{ _m02, _m12, _m22, _m32 }, \
			{ _m03, _m13, _m23, _m33 } \
		} \
	}
#define s_mat4_identity (s_mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f))
static inline s_mat4 s_mat4_mul(const s_mat4* _mat_a, const s_mat4* _mat_b) {
    s_mat4 result;
    for (u8 c = 0; c < 4; c++) {
        for (u8 r = 0; r < 4; r++) {
            result.m[c][r] = 0.0f;
            for (u8 k = 0; k < 4; k++) {
                result.m[c][r] += _mat_a->m[k][r] * _mat_b->m[c][k];
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
static inline b8 s_mat4_inverse_checked(const s_mat4* _mat, s_mat4* _out) {
    if (_mat == NULL || _out == NULL) return false;
    s_real aug[4][8];
    for (u8 i = 0; i < 4; i++) {
        for (u8 j = 0; j < 4; j++) aug[i][j] = _mat->m[j][i];
        for (u8 j = 0; j < 4; j++) aug[i][j + 4] = (i == j) ? 1.0f : 0.0f;
    }
    for (u8 i = 0; i < 4; i++) {
        u8 pivot_row = i;
        s_real pivot_abs = s_real_abs(aug[i][i]);
        for (u8 r = (u8)(i + 1); r < 4; ++r) {
            s_real value_abs = s_real_abs(aug[r][i]);
            if (value_abs > pivot_abs) {
                pivot_abs = value_abs;
                pivot_row = r;
            }
        }
        if (pivot_abs <= S_EPSILON) return false;
        if (pivot_row != i) {
            for (u8 j = 0; j < 8; ++j) {
                s_real tmp = aug[i][j];
                aug[i][j] = aug[pivot_row][j];
                aug[pivot_row][j] = tmp;
            }
        }
        s_real pivot = aug[i][i];
        for (u8 j = 0; j < 8; j++) aug[i][j] /= pivot;
        for (u8 k = 0; k < 4; k++) {
            if (k != i) {
                s_real factor = aug[k][i];
                for (u8 j = 0; j < 8; j++) aug[k][j] -= factor * aug[i][j];
            }
        }
    }
    for (u8 i = 0; i < 4; i++)
        for (u8 j = 0; j < 4; j++)
            _out->m[j][i] = aug[i][j + 4];
    return true;
}
static inline s_mat4 s_mat4_inverse(const s_mat4* _mat) {
    s_mat4 inv = s_mat4_identity;
    if (!s_mat4_inverse_checked(_mat, &inv)) return s_mat4_identity;
    return inv;
}
#define s_mat4_translate(_mat, _v)          (s_mat4_mul((_mat), &(s_mat4(1.0f, 0.0f, 0.0f, (_v)->x, 0.0f, 1.0f, 0.0f, (_v)->y, 0.0f, 0.0f, 1.0f, (_v)->z, 0.0f, 0.0f, 0.0f, 1.0f))))
#define s_mat4_set_translation(_mat, _v)    { (_mat)->m[3][0] = (_v)->x; (_mat)->m[3][1] = (_v)->y; (_mat)->m[3][2] = (_v)->z; }
#define s_mat4_get_translation(_mat)        (s_vec3((_mat)->m[3][0], (_mat)->m[3][1], (_mat)->m[3][2]))
#define s_mat4_rotate_x(_mat, _angle)       (s_mat4_mul((_mat), &(s_mat4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, s_real_cos(_angle), -s_real_sin(_angle), 0.0f, 0.0f, s_real_sin(_angle), s_real_cos(_angle), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f))))
#define s_mat4_set_rotation_x(_mat, _angle) { (_mat)->m[1][1] = s_real_cos(_angle); (_mat)->m[1][2] = s_real_sin(_angle); (_mat)->m[2][1] = -s_real_sin(_angle); (_mat)->m[2][2] = s_real_cos(_angle); }
#define s_mat4_get_rotation_x(_mat)         (s_real_atan2((_mat)->m[1][2], (_mat)->m[1][1]))
#define s_mat4_rotate_y(_mat, _angle)       (s_mat4_mul((_mat), &(s_mat4(s_real_cos(_angle), 0.0f, s_real_sin(_angle), 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, -s_real_sin(_angle), 0.0f, s_real_cos(_angle), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f))))
#define s_mat4_set_rotation_y(_mat, _angle) { (_mat)->m[0][0] = s_real_cos(_angle); (_mat)->m[2][0] = s_real_sin(_angle); (_mat)->m[0][2] = -s_real_sin(_angle); (_mat)->m[2][2] = s_real_cos(_angle); }
#define s_mat4_get_rotation_y(_mat)         (s_real_atan2((_mat)->m[2][0], (_mat)->m[0][0]))
#define s_mat4_rotate_z(_mat, _angle)       (s_mat4_mul((_mat), &(s_mat4(s_real_cos(_angle), -s_real_sin(_angle), 0.0f, 0.0f, s_real_sin(_angle), s_real_cos(_angle), 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f))))
#define s_mat4_set_rotation_z(_mat, _angle) { (_mat)->m[0][0] = s_real_cos(_angle); (_mat)->m[0][1] = s_real_sin(_angle); (_mat)->m[1][0] = -s_real_sin(_angle); (_mat)->m[1][1] = s_real_cos(_angle); }
#define s_mat4_get_rotation_z(_mat)         (s_real_atan2((_mat)->m[0][1], (_mat)->m[0][0]))
#define s_mat4_scale(_mat, _v)              (s_mat4_mul((_mat), &(s_mat4((_v)->x, 0.0f, 0.0f, 0.0f, 0.0f, (_v)->y, 0.0f, 0.0f, 0.0f, 0.0f, (_v)->z, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f))))
#define s_mat4_set_scale(_mat, _v)          { (_mat)->m[0][0] = (_v)->x; (_mat)->m[1][1] = (_v)->y; (_mat)->m[2][2] = (_v)->z; }
#define s_mat4_get_scale(_mat)              (s_vec3((_mat)->m[0][0], (_mat)->m[1][1], (_mat)->m[2][2]))
static inline s_mat4 s_mat4_look_at(const s_vec3* _from, const s_vec3* _to, const s_vec3* _up) {
    if (_from == NULL || _to == NULL || _up == NULL) return s_mat4_identity;
    const s_real eps = S_EPSILON;
    s_vec3 f = s_vec3_sub(_to, _from);
    if (s_vec3_length(&f) <= eps) {
        f = s_vec3(0.0f, 0.0f, -1.0f);
    } else {
        f = s_vec3_normalize(&f);
    }

    s_vec3 up = *_up;
    if (s_vec3_length(&up) <= eps) {
        up = s_vec3(0.0f, 1.0f, 0.0f);
    } else {
        up = s_vec3_normalize(&up);
    }

    s_vec3 s = s_vec3_cross(&f, &up);
    if (s_vec3_length(&s) <= eps) {
        const s_vec3 fallback_up = (s_real_abs(f.y) > 0.999f)
            ? s_vec3(0.0f, 0.0f, 1.0f)
            : s_vec3(0.0f, 1.0f, 0.0f);
        s = s_vec3_cross(&f, &fallback_up);
    }
    s = s_vec3_normalize(&s);
    s_vec3 u = s_vec3_cross(&s, &f);
    u = s_vec3_normalize(&u);
    return s_mat4(
		s.x, s.y, s.z, -s_vec3_dot(&s, _from),
		u.x, u.y, u.z, -s_vec3_dot(&u, _from),
		-f.x, -f.y, -f.z, s_vec3_dot(&f, _from),
		0.0f, 0.0f, 0.0f, 1.0f
    );
}
static inline s_mat4 s_mat4_perspective(const s_real _fov, const s_real _aspect, const s_real _near, const s_real _far) {
    s_real tanHalfFov = s_real_tan(_fov / 2.0f);
    return s_mat4(
        1.0f / (_aspect * tanHalfFov), 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f / tanHalfFov, 0.0f, 0.0f,
        0.0f, 0.0f, -(_far + _near) / (_far - _near), -(2.0f * _far * _near) / (_far - _near),
        0.0f, 0.0f, -1.0f, 0.0f
    );
}
static inline s_mat4 s_mat4_ortho(const s_real _left, const s_real _right, const s_real _bottom, const s_real _top, const s_real _near, const s_real _far) {
    return s_mat4(
        2.0f / (_right - _left), 0.0f, 0.0f, -(_right + _left) / (_right - _left),
        0.0f, 2.0f / (_top - _bottom), 0.0f, -(_top + _bottom) / (_top - _bottom),
        0.0f, 0.0f, -2.0f / (_far - _near), -(_far + _near) / (_far - _near),
        0.0f, 0.0f, 0.0f, 1.0f
    );
}
typedef struct { s_vec2 min, max; } s_box_2d;
typedef struct { s_vec3 min, max; } s_box_3d;
typedef struct { s_vec2 position; s_real radius; } s_circle;
typedef struct { s_vec3 position; s_real radius; } s_sphere;

static inline s_real s_clamp(const s_real value, const s_real min_value, const s_real max_value) {
	if (value < min_value) {
		return min_value;
	}
	if (value > max_value) {
		return max_value;
	}
	return value;
}

static inline s_real s_clamp01(const s_real value) {
	return s_clamp(value, 0.0f, 1.0f);
}

static inline s_real s_min3(const s_real a, const s_real b, const s_real c) {
	return s_real_min(a, s_real_min(b, c));
}

static inline s_real s_max3(const s_real a, const s_real b, const s_real c) {
	return s_real_max(a, s_real_max(b, c));
}

static inline s_real s_vec2_length_sq(const s_vec2* v) {
	return v ? (v->x * v->x) + (v->y * v->y) : 0.0f;
}

static inline s_real s_vec3_length_sq(const s_vec3* v) {
	return v ? (v->x * v->x) + (v->y * v->y) + (v->z * v->z) : 0.0f;
}

static inline s_real s_vec4_length_sq(const s_vec4* v) {
	return v ? (v->x * v->x) + (v->y * v->y) + (v->z * v->z) + (v->w * v->w) : 0.0f;
}

static inline s_vec2 s_vec2_clamp(const s_vec2* v, const s_vec2* min_v, const s_vec2* max_v) {
	if (!v || !min_v || !max_v) {
		return s_vec2(0.0f, 0.0f);
	}
	return s_vec2(s_clamp(v->x, min_v->x, max_v->x), s_clamp(v->y, min_v->y, max_v->y));
}

static inline s_vec3 s_vec3_clamp(const s_vec3* v, const s_vec3* min_v, const s_vec3* max_v) {
	if (!v || !min_v || !max_v) {
		return s_vec3(0.0f, 0.0f, 0.0f);
	}
	return s_vec3(
		s_clamp(v->x, min_v->x, max_v->x),
		s_clamp(v->y, min_v->y, max_v->y),
		s_clamp(v->z, min_v->z, max_v->z)
	);
}

static inline s_vec2 s_vec2_perp(const s_vec2* v) {
	return v ? s_vec2(-v->y, v->x) : s_vec2(0.0f, 0.0f);
}

static inline s_vec2 s_vec2_rotate(const s_vec2* v, const s_real angle) {
	if (!v) {
		return s_vec2(0.0f, 0.0f);
	}
	const s_real c = s_real_cos(angle);
	const s_real s = s_real_sin(angle);
	return s_vec2((v->x * c) - (v->y * s), (v->x * s) + (v->y * c));
}

static inline s_vec2 s_vec2_rotate_inv(const s_vec2* v, const s_real angle) {
	if (!v) {
		return s_vec2(0.0f, 0.0f);
	}
	const s_real c = s_real_cos(angle);
	const s_real s = s_real_sin(angle);
	return s_vec2((v->x * c) + (v->y * s), (-v->x * s) + (v->y * c));
}

static inline s_vec3 s_vec3_rotate(const s_vec3* v, const s_vec3* rotation) {
	if (!v || !rotation) {
		return s_vec3(0.0f, 0.0f, 0.0f);
	}
	const s_real cx = s_real_cos(rotation->x);
	const s_real sx = s_real_sin(rotation->x);
	const s_real cy = s_real_cos(rotation->y);
	const s_real sy = s_real_sin(rotation->y);
	const s_real cz = s_real_cos(rotation->z);
	const s_real sz = s_real_sin(rotation->z);
	const s_real m00 = cz * cy;
	const s_real m01 = (cz * sy * sx) - (sz * cx);
	const s_real m02 = (cz * sy * cx) + (sz * sx);
	const s_real m10 = sz * cy;
	const s_real m11 = (sz * sy * sx) + (cz * cx);
	const s_real m12 = (sz * sy * cx) - (cz * sx);
	const s_real m20 = -sy;
	const s_real m21 = cy * sx;
	const s_real m22 = cy * cx;
	return s_vec3(
		(v->x * m00) + (v->y * m01) + (v->z * m02),
		(v->x * m10) + (v->y * m11) + (v->z * m12),
		(v->x * m20) + (v->y * m21) + (v->z * m22)
	);
}

static inline s_vec3 s_vec3_rotate_inv(const s_vec3* v, const s_vec3* rotation) {
	if (!v || !rotation) {
		return s_vec3(0.0f, 0.0f, 0.0f);
	}
	const s_real cx = s_real_cos(rotation->x);
	const s_real sx = s_real_sin(rotation->x);
	const s_real cy = s_real_cos(rotation->y);
	const s_real sy = s_real_sin(rotation->y);
	const s_real cz = s_real_cos(rotation->z);
	const s_real sz = s_real_sin(rotation->z);
	const s_real m00 = cz * cy;
	const s_real m01 = (cz * sy * sx) - (sz * cx);
	const s_real m02 = (cz * sy * cx) + (sz * sx);
	const s_real m10 = sz * cy;
	const s_real m11 = (sz * sy * sx) + (cz * cx);
	const s_real m12 = (sz * sy * cx) - (cz * sx);
	const s_real m20 = -sy;
	const s_real m21 = cy * sx;
	const s_real m22 = cy * cx;
	return s_vec3(
		(v->x * m00) + (v->y * m10) + (v->z * m20),
		(v->x * m01) + (v->y * m11) + (v->z * m21),
		(v->x * m02) + (v->y * m12) + (v->z * m22)
	);
}

static inline s_vec3 s_vec3_rotate_axis(const s_vec3* v, const s_vec3* axis, const s_real radians) {
	if (!v || !axis) {
		return s_vec3(0.0f, 0.0f, 0.0f);
	}
	const s_real axis_len = s_vec3_length(axis);
	if (axis_len <= S_EPSILON || s_real_abs(radians) <= S_EPSILON) {
		return *v;
	}
	const s_vec3 unit_axis = s_vec3_divs(axis, axis_len);
	const s_real c = s_real_cos(radians);
	const s_real s = s_real_sin(radians);
	const s_vec3 term_a = s_vec3_muls(v, c);
	const s_vec3 cross = s_vec3_cross(&unit_axis, v);
	const s_vec3 term_b = s_vec3_muls(&cross, s);
	const s_vec3 term_c = s_vec3_muls(&unit_axis, s_vec3_dot(&unit_axis, v) * (1.0f - c));
	const s_vec3 ab = s_vec3_add(&term_a, &term_b);
	return s_vec3_add(&ab, &term_c);
}

static inline void s_vec3_rotation_axes(const s_vec3* rotation, s_vec3* out_x_axis, s_vec3* out_y_axis, s_vec3* out_z_axis) {
	const s_vec3 x = s_vec3(1.0f, 0.0f, 0.0f);
	const s_vec3 y = s_vec3(0.0f, 1.0f, 0.0f);
	const s_vec3 z = s_vec3(0.0f, 0.0f, 1.0f);
	if (out_x_axis) {
		*out_x_axis = rotation ? s_vec3_rotate(&x, rotation) : x;
	}
	if (out_y_axis) {
		*out_y_axis = rotation ? s_vec3_rotate(&y, rotation) : y;
	}
	if (out_z_axis) {
		*out_z_axis = rotation ? s_vec3_rotate(&z, rotation) : z;
	}
}

static inline s_vec4 s_mat4_mul_vec4(const s_mat4* m, const s_vec4* v) {
	if (!m || !v) {
		return s_vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	return s_vec4(
		(m->m[0][0] * v->x) + (m->m[1][0] * v->y) + (m->m[2][0] * v->z) + (m->m[3][0] * v->w),
		(m->m[0][1] * v->x) + (m->m[1][1] * v->y) + (m->m[2][1] * v->z) + (m->m[3][1] * v->w),
		(m->m[0][2] * v->x) + (m->m[1][2] * v->y) + (m->m[2][2] * v->z) + (m->m[3][2] * v->w),
		(m->m[0][3] * v->x) + (m->m[1][3] * v->y) + (m->m[2][3] * v->z) + (m->m[3][3] * v->w)
	);
}

static inline s_vec3 s_mat4_mul_point(const s_mat4* transform, const s_vec3* point) {
	if (!transform || !point) {
		return s_vec3(0.0f, 0.0f, 0.0f);
	}
	const s_real x = (transform->m[0][0] * point->x) + (transform->m[1][0] * point->y) + (transform->m[2][0] * point->z) + transform->m[3][0];
	const s_real y = (transform->m[0][1] * point->x) + (transform->m[1][1] * point->y) + (transform->m[2][1] * point->z) + transform->m[3][1];
	const s_real z = (transform->m[0][2] * point->x) + (transform->m[1][2] * point->y) + (transform->m[2][2] * point->z) + transform->m[3][2];
	const s_real w = (transform->m[0][3] * point->x) + (transform->m[1][3] * point->y) + (transform->m[2][3] * point->z) + transform->m[3][3];
	if (s_real_abs(w) > S_EPSILON) {
		return s_vec3(x / w, y / w, z / w);
	}
	return s_vec3(x, y, z);
}

static inline s_vec3 s_mat4_extract_basis_scale(const s_mat4* transform) {
	if (!transform) {
		return s_vec3(1.0f, 1.0f, 1.0f);
	}
	return s_vec3(
		s_real_sqrt((transform->m[0][0] * transform->m[0][0]) + (transform->m[0][1] * transform->m[0][1]) + (transform->m[0][2] * transform->m[0][2])),
		s_real_sqrt((transform->m[1][0] * transform->m[1][0]) + (transform->m[1][1] * transform->m[1][1]) + (transform->m[1][2] * transform->m[1][2])),
		s_real_sqrt((transform->m[2][0] * transform->m[2][0]) + (transform->m[2][1] * transform->m[2][1]) + (transform->m[2][2] * transform->m[2][2]))
	);
}

static inline s_mat4 s_mat4_extract_rotation(const s_mat4* transform, const s_vec3* scale) {
	s_mat4 rotation = s_mat4_identity;
	if (!transform || !scale) {
		return rotation;
	}
	if (s_real_abs(scale->x) > S_EPSILON) {
		rotation.m[0][0] = transform->m[0][0] / scale->x;
		rotation.m[0][1] = transform->m[0][1] / scale->x;
		rotation.m[0][2] = transform->m[0][2] / scale->x;
	}
	if (s_real_abs(scale->y) > S_EPSILON) {
		rotation.m[1][0] = transform->m[1][0] / scale->y;
		rotation.m[1][1] = transform->m[1][1] / scale->y;
		rotation.m[1][2] = transform->m[1][2] / scale->y;
	}
	if (s_real_abs(scale->z) > S_EPSILON) {
		rotation.m[2][0] = transform->m[2][0] / scale->z;
		rotation.m[2][1] = transform->m[2][1] / scale->z;
		rotation.m[2][2] = transform->m[2][2] / scale->z;
	}
	return rotation;
}

static inline s_mat4 s_mat4_rotation_xyz(const s_vec3* rotation) {
	if (!rotation) {
		return s_mat4_identity;
	}
	const s_real cx = s_real_cos(rotation->x);
	const s_real sx = s_real_sin(rotation->x);
	const s_real cy = s_real_cos(rotation->y);
	const s_real sy = s_real_sin(rotation->y);
	const s_real cz = s_real_cos(rotation->z);
	const s_real sz = s_real_sin(rotation->z);
	return s_mat4(
		cy * cz,
		(sx * sy * cz) + (cx * sz),
		(-cx * sy * cz) + (sx * sz),
		0.0f,
		-cy * sz,
		(-sx * sy * sz) + (cx * cz),
		(cx * sy * sz) + (sx * cz),
		0.0f,
		sy,
		-sx * cy,
		cx * cy,
		0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	);
}

static inline void s_mat4_apply_basis_scale(s_mat4* transform, const s_vec3* scale) {
	if (!transform || !scale) {
		return;
	}
	transform->m[0][0] *= scale->x;
	transform->m[0][1] *= scale->x;
	transform->m[0][2] *= scale->x;
	transform->m[1][0] *= scale->y;
	transform->m[1][1] *= scale->y;
	transform->m[1][2] *= scale->y;
	transform->m[2][0] *= scale->z;
	transform->m[2][1] *= scale->z;
	transform->m[2][2] *= scale->z;
}

static inline s_box_2d s_box_2d_from_edges(const s_real left, const s_real bottom, const s_real right, const s_real top) {
	s_box_2d box = {0};
	box.min = s_vec2(s_real_min(left, right), s_real_min(bottom, top));
	box.max = s_vec2(s_real_max(left, right), s_real_max(bottom, top));
	return box;
}

static inline s_box_3d s_box_3d_from_edges(const s_real left, const s_real bottom, const s_real back, const s_real right, const s_real top, const s_real front) {
	s_box_3d box = {0};
	box.min = s_vec3(s_real_min(left, right), s_real_min(bottom, top), s_real_min(back, front));
	box.max = s_vec3(s_real_max(left, right), s_real_max(bottom, top), s_real_max(back, front));
	return box;
}

static inline void s_box_2d_make(s_box_2d* out_box, const s_mat3* transform) {
	if (!out_box || !transform) {
		return;
	}
	const s_vec2 pos = s_mat3_get_translation(transform);
	const s_vec2 scale = s_mat3_get_scale(transform);
	out_box->min = s_vec2(pos.x - scale.x, -pos.y - scale.y);
	out_box->max = s_vec2(pos.x + scale.x, -pos.y + scale.y);
}

static inline s_box_2d s_box_2d_empty(void);
static inline s_box_3d s_box_3d_empty(void);

static inline s_box_2d s_box_2d_clean(const s_box_2d* box) {
	if (!box) {
		return s_box_2d_empty();
	}
    if (box->min.x > box->max.x || box->min.y > box->max.y) return s_box_2d_empty();
	return s_box_2d_from_edges(box->min.x, box->min.y, box->max.x, box->max.y);
}

static inline s_box_3d s_box_3d_clean(const s_box_3d* box) {
	if (!box) {
		return s_box_3d_empty();
	}
    if (box->min.x > box->max.x || box->min.y > box->max.y || box->min.z > box->max.z) return s_box_3d_empty();
	return s_box_3d_from_edges(box->min.x, box->min.y, box->min.z, box->max.x, box->max.y, box->max.z);
}

static inline s_box_2d s_box_2d_empty(void) {
	s_box_2d box = {0};
	box.min = s_vec2((s_real)INFINITY, (s_real)INFINITY);
	box.max = s_vec2((s_real)-INFINITY, (s_real)-INFINITY);
	return box;
}

static inline s_box_3d s_box_3d_empty(void) {
	s_box_3d box = {0};
	box.min = s_vec3((s_real)INFINITY, (s_real)INFINITY, (s_real)INFINITY);
	box.max = s_vec3((s_real)-INFINITY, (s_real)-INFINITY, (s_real)-INFINITY);
	return box;
}

static inline b8 s_box_2d_is_empty(const s_box_2d* box) {
	if (!box) {
		return true;
	}
	return box->min.x > box->max.x || box->min.y > box->max.y ||
        box->max.x <= box->min.x + S_EPSILON || box->max.y <= box->min.y + S_EPSILON;
}

static inline b8 s_box_3d_is_empty(const s_box_3d* box) {
	if (!box) {
		return true;
	}
	return box->min.x > box->max.x || box->min.y > box->max.y || box->min.z > box->max.z ||
        box->max.x <= box->min.x + S_EPSILON ||
		box->max.y <= box->min.y + S_EPSILON ||
		box->max.z <= box->min.z + S_EPSILON;
}

static inline void s_box_2d_add_point(s_box_2d* out_box, const s_vec2* point) {
	if (!out_box || !point) {
		return;
	}
	out_box->min.x = s_real_min(out_box->min.x, point->x);
	out_box->min.y = s_real_min(out_box->min.y, point->y);
	out_box->max.x = s_real_max(out_box->max.x, point->x);
	out_box->max.y = s_real_max(out_box->max.y, point->y);
}

static inline void s_box_3d_add_point(s_box_3d* out_box, const s_vec3* point) {
	if (!out_box || !point) {
		return;
	}
	out_box->min.x = s_real_min(out_box->min.x, point->x);
	out_box->min.y = s_real_min(out_box->min.y, point->y);
	out_box->min.z = s_real_min(out_box->min.z, point->z);
	out_box->max.x = s_real_max(out_box->max.x, point->x);
	out_box->max.y = s_real_max(out_box->max.y, point->y);
	out_box->max.z = s_real_max(out_box->max.z, point->z);
}

static inline void s_box_2d_add(s_box_2d* out_box, const s_box_2d* box) {
	if (!out_box || !box) {
		return;
	}
	const s_box_2d clean = s_box_2d_clean(box);
    if (s_box_2d_is_empty(&clean)) return;
	s_box_2d_add_point(out_box, &clean.min);
	s_box_2d_add_point(out_box, &clean.max);
}

static inline void s_box_3d_add(s_box_3d* out_box, const s_box_3d* box) {
	if (!out_box || !box) {
		return;
	}
	const s_box_3d clean = s_box_3d_clean(box);
    if (s_box_3d_is_empty(&clean)) return;
	s_box_3d_add_point(out_box, &clean.min);
	s_box_3d_add_point(out_box, &clean.max);
}

static inline void s_box_2d_expand(s_box_2d* box, const s_real amount) {
	if (!box || s_box_2d_is_empty(box) || amount <= 0.0f) {
		return;
	}
	const s_vec2 padding = s_vec2(amount, amount);
	box->min = s_vec2_sub(&box->min, &padding);
	box->max = s_vec2_add(&box->max, &padding);
}

static inline void s_box_3d_expand(s_box_3d* box, const s_real amount) {
	if (!box || s_box_3d_is_empty(box) || amount <= 0.0f) {
		return;
	}
	const s_vec3 padding = s_vec3(amount, amount, amount);
	box->min = s_vec3_sub(&box->min, &padding);
	box->max = s_vec3_add(&box->max, &padding);
}

static inline s_box_2d s_box_2d_translate(const s_box_2d* box, const s_real dx, const s_real dy) {
	if (!box || s_box_2d_is_empty(box)) return s_box_2d_empty();
	return s_box_2d_from_edges(box->min.x + dx, box->min.y + dy, box->max.x + dx, box->max.y + dy);
}

static inline s_box_3d s_box_3d_translate(const s_box_3d* box, const s_real dx, const s_real dy, const s_real dz) {
	if (!box || s_box_3d_is_empty(box)) return s_box_3d_empty();
	return s_box_3d_from_edges(box->min.x + dx, box->min.y + dy, box->min.z + dz, box->max.x + dx, box->max.y + dy, box->max.z + dz);
}

static inline s_real s_box_2d_width(const s_box_2d* box) {
	return box && !s_box_2d_is_empty(box) ? (box->max.x - box->min.x) : 0.0f;
}

static inline s_real s_box_2d_height(const s_box_2d* box) {
	return box && !s_box_2d_is_empty(box) ? (box->max.y - box->min.y) : 0.0f;
}

static inline s_real s_box_3d_width(const s_box_3d* box) {
	return box && !s_box_3d_is_empty(box) ? (box->max.x - box->min.x) : 0.0f;
}

static inline s_real s_box_3d_height(const s_box_3d* box) {
	return box && !s_box_3d_is_empty(box) ? (box->max.y - box->min.y) : 0.0f;
}

static inline s_real s_box_3d_depth(const s_box_3d* box) {
	return box && !s_box_3d_is_empty(box) ? (box->max.z - box->min.z) : 0.0f;
}

static inline b8 s_box_2d_contains(const s_box_2d* box, const s_vec2* point) {
	if (!box || !point || s_box_2d_is_empty(box)) {
		return false;
	}
	return point->x >= box->min.x && point->x <= box->max.x &&
		point->y >= box->min.y && point->y <= box->max.y;
}

static inline b8 s_box_3d_contains(const s_box_3d* box, const s_vec3* point) {
	if (!box || !point || s_box_3d_is_empty(box)) {
		return false;
	}
	return point->x >= box->min.x && point->x <= box->max.x &&
		point->y >= box->min.y && point->y <= box->max.y &&
		point->z >= box->min.z && point->z <= box->max.z;
}

static inline b8 s_box_2d_intersects(const s_box_2d* a, const s_box_2d* b) {
	if (!a || !b || s_box_2d_is_empty(a) || s_box_2d_is_empty(b)) {
		return false;
	}
	return a->min.x <= b->max.x && a->max.x >= b->min.x &&
		a->min.y <= b->max.y && a->max.y >= b->min.y;
}

static inline b8 s_box_3d_intersects(const s_box_3d* a, const s_box_3d* b) {
	if (!a || !b || s_box_3d_is_empty(a) || s_box_3d_is_empty(b)) {
		return false;
	}
	return a->min.x <= b->max.x && a->max.x >= b->min.x &&
		a->min.y <= b->max.y && a->max.y >= b->min.y &&
		a->min.z <= b->max.z && a->max.z >= b->min.z;
}

static inline s_box_2d s_box_2d_intersection(const s_box_2d* a, const s_box_2d* b) {
	if (!s_box_2d_intersects(a, b)) return s_box_2d_empty();
	return s_box_2d_from_edges(
		s_real_max(a->min.x, b->min.x),
		s_real_max(a->min.y, b->min.y),
		s_real_min(a->max.x, b->max.x),
		s_real_min(a->max.y, b->max.y)
	);
}

static inline s_box_3d s_box_3d_intersection(const s_box_3d* a, const s_box_3d* b) {
	if (!s_box_3d_intersects(a, b)) return s_box_3d_empty();
	return s_box_3d_from_edges(
		s_real_max(a->min.x, b->min.x),
		s_real_max(a->min.y, b->min.y),
		s_real_max(a->min.z, b->min.z),
		s_real_min(a->max.x, b->max.x),
		s_real_min(a->max.y, b->max.y),
		s_real_min(a->max.z, b->max.z)
	);
}

static inline u32 s_box_2d_largest_axis(const s_box_2d* box) {
	if (!box || s_box_2d_is_empty(box)) {
		return 0u;
	}
	const s_real extent_x = box->max.x - box->min.x;
	const s_real extent_y = box->max.y - box->min.y;
	return extent_x >= extent_y ? 0u : 1u;
}

static inline u32 s_box_3d_largest_axis(const s_box_3d* box) {
	if (!box || s_box_3d_is_empty(box)) {
		return 0u;
	}
	const s_real extent_x = box->max.x - box->min.x;
	const s_real extent_y = box->max.y - box->min.y;
	const s_real extent_z = box->max.z - box->min.z;
	if (extent_x >= extent_y && extent_x >= extent_z) {
		return 0u;
	}
	return extent_y >= extent_z ? 1u : 2u;
}

static inline s_box_2d s_box_2d_transform(const s_box_2d* box, const s_mat3* matrix) {
	if (!box || !matrix || s_box_2d_is_empty(box)) {
		return s_box_2d_empty();
	}
	const s_box_2d clean = s_box_2d_clean(box);
	const s_vec2 corners[4] = {
		clean.min,
		s_vec2(clean.max.x, clean.min.y),
		clean.max,
		s_vec2(clean.min.x, clean.max.y)
	};
	s_box_2d out_box = s_box_2d_empty();
	for (u32 i = 0u; i < 4u; ++i) {
		const s_vec2 p = corners[i];
		const s_vec2 transformed = s_vec2(
			(matrix->m[0][0] * p.x) + (matrix->m[1][0] * p.y) + matrix->m[2][0],
			(matrix->m[0][1] * p.x) + (matrix->m[1][1] * p.y) + matrix->m[2][1]
		);
		s_box_2d_add_point(&out_box, &transformed);
	}
	return out_box;
}

static inline s_box_3d s_box_3d_transform(const s_box_3d* box, const s_mat4* matrix) {
	if (!box || !matrix || s_box_3d_is_empty(box)) {
		return s_box_3d_empty();
	}
	const s_box_3d clean = s_box_3d_clean(box);
	const s_vec3 corners[8] = {
		s_vec3(clean.min.x, clean.min.y, clean.min.z),
		s_vec3(clean.max.x, clean.min.y, clean.min.z),
		s_vec3(clean.max.x, clean.max.y, clean.min.z),
		s_vec3(clean.min.x, clean.max.y, clean.min.z),
		s_vec3(clean.min.x, clean.min.y, clean.max.z),
		s_vec3(clean.max.x, clean.min.y, clean.max.z),
		s_vec3(clean.max.x, clean.max.y, clean.max.z),
		s_vec3(clean.min.x, clean.max.y, clean.max.z)
	};
	s_box_3d out_box = s_box_3d_empty();
	for (u32 i = 0u; i < 8u; ++i) {
		const s_vec3 transformed = s_mat4_mul_point(matrix, &corners[i]);
		s_box_3d_add_point(&out_box, &transformed);
	}
	return out_box;
}

static inline b8 s_circle_intersects(const s_circle* a, const s_circle* b) {
	if (!a || !b) {
		return false;
	}
	const s_vec2 delta = s_vec2_sub(&a->position, &b->position);
	const s_real radius = a->radius + b->radius;
	return s_vec2_length_sq(&delta) <= radius * radius;
}

static inline b8 s_sphere_intersects(const s_sphere* a, const s_sphere* b) {
	if (!a || !b) {
		return false;
	}
	const s_vec3 delta = s_vec3_sub(&a->position, &b->position);
	const s_real radius = a->radius + b->radius;
	return s_vec3_length_sq(&delta) <= radius * radius;
}

static inline b8 s_ray_intersects_box_2d(const s_vec2* origin, const s_vec2* direction, const s_box_2d* box, s_real* out_t) {
	if (!origin || !direction || !box || s_box_2d_is_empty(box)) {
		return false;
	}
	s_real tmin = 0.0f;
	s_real tmax = (s_real)INFINITY;
	const s_real o[2] = { origin->x, origin->y };
	const s_real d[2] = { direction->x, direction->y };
	const s_real bmin[2] = { box->min.x, box->min.y };
	const s_real bmax[2] = { box->max.x, box->max.y };
	for (u32 i = 0u; i < 2u; ++i) {
		if (s_real_abs(d[i]) < S_EPSILON) {
			if (o[i] < bmin[i] || o[i] > bmax[i]) return false;
			continue;
		}
		const s_real inv = 1.0f / d[i];
		s_real t1 = (bmin[i] - o[i]) * inv;
		s_real t2 = (bmax[i] - o[i]) * inv;
		if (t1 > t2) {
			const s_real tmp = t1;
			t1 = t2;
			t2 = tmp;
		}
		tmin = s_real_max(tmin, t1);
		tmax = s_real_min(tmax, t2);
		if (tmin > tmax) {
			return false;
		}
	}
	if (out_t) {
		*out_t = tmin;
	}
	return true;
}

static inline b8 s_ray_intersects_box_3d(const s_vec3* origin, const s_vec3* direction, const s_box_3d* box, s_real* out_t) {
	if (!origin || !direction || !box || s_box_3d_is_empty(box)) {
		return false;
	}
	s_real tmin = 0.0f;
	s_real tmax = (s_real)INFINITY;
	const s_real o[3] = { origin->x, origin->y, origin->z };
	const s_real d[3] = { direction->x, direction->y, direction->z };
	const s_real bmin[3] = { box->min.x, box->min.y, box->min.z };
	const s_real bmax[3] = { box->max.x, box->max.y, box->max.z };
	for (u32 i = 0u; i < 3u; ++i) {
		if (s_real_abs(d[i]) < S_EPSILON) {
			if (o[i] < bmin[i] || o[i] > bmax[i]) {
				return false;
			}
			continue;
		}
		const s_real inv = 1.0f / d[i];
		s_real t1 = (bmin[i] - o[i]) * inv;
		s_real t2 = (bmax[i] - o[i]) * inv;
		if (t1 > t2) {
			const s_real tmp = t1;
			t1 = t2;
			t2 = tmp;
		}
		tmin = s_real_max(tmin, t1);
		tmax = s_real_min(tmax, t2);
		if (tmin > tmax) {
			return false;
		}
	}
	if (out_t) {
		*out_t = tmin;
	}
	return true;
}

static inline b8 s_ray_intersects_circle_2d(const s_vec2* origin, const s_vec2* direction, const s_real max_distance, const s_vec2* center, const s_real radius, s_real* out_t, s_vec2* out_normal) {
	if (!origin || !direction || !center || radius <= 0.0f || max_distance <= 0.0f) {
		return false;
	}
	const s_vec2 m = s_vec2(origin->x - center->x, origin->y - center->y);
	const s_real a = s_vec2_dot(direction, direction);
	if (a <= S_EPSILON) {
		return false;
	}
	const s_real b = s_vec2_dot(&m, direction);
	const s_real c = s_vec2_dot(&m, &m) - (radius * radius);
	const s_real disc = (b * b) - (a * c);
	if (disc < 0.0f) {
		return false;
	}
	const s_real sqrt_disc = s_real_sqrt(disc);
	s_real t = (-b - sqrt_disc) / a;
	if (t < 0.0f) {
		t = (-b + sqrt_disc) / a;
	}
	if (t < 0.0f || t > max_distance) {
		return false;
	}
	if (out_t) {
		*out_t = t;
	}
	if (out_normal) {
		const s_vec2 hit = s_vec2(origin->x + direction->x * t, origin->y + direction->y * t);
		const s_vec2 diff = s_vec2(hit.x - center->x, hit.y - center->y);
		const s_real len = s_vec2_length(&diff);
		*out_normal = len < S_EPSILON ? s_vec2(0.0f, 1.0f) : s_vec2(diff.x / len, diff.y / len);
	}
	return true;
}

static inline b8 s_ray_intersects_sphere_3d(const s_vec3* origin, const s_vec3* direction, const s_real max_distance, const s_vec3* center, const s_real radius, s_real* out_t, s_vec3* out_normal) {
	if (!origin || !direction || !center || radius <= 0.0f || max_distance <= 0.0f) {
		return false;
	}
	const s_vec3 m = s_vec3(origin->x - center->x, origin->y - center->y, origin->z - center->z);
	const s_real a = s_vec3_dot(direction, direction);
	if (a <= S_EPSILON) {
		return false;
	}
	const s_real b = s_vec3_dot(&m, direction);
	const s_real c = s_vec3_dot(&m, &m) - (radius * radius);
	const s_real disc = (b * b) - (a * c);
	if (disc < 0.0f) {
		return false;
	}
	const s_real sqrt_disc = s_real_sqrt(disc);
	s_real t = (-b - sqrt_disc) / a;
	if (t < 0.0f) {
		t = (-b + sqrt_disc) / a;
	}
	if (t < 0.0f || t > max_distance) {
		return false;
	}
	if (out_t) {
		*out_t = t;
	}
	if (out_normal) {
		const s_vec3 hit = s_vec3(origin->x + direction->x * t, origin->y + direction->y * t, origin->z + direction->z * t);
		const s_vec3 n = s_vec3(hit.x - center->x, hit.y - center->y, hit.z - center->z);
		const s_real len = s_vec3_length(&n);
		*out_normal = len < S_EPSILON ? s_vec3(0.0f, 1.0f, 0.0f) : s_vec3_divs(&n, len);
	}
	return true;
}

static inline b8 s_ray_intersects_triangle_3d(const s_vec3* origin, const s_vec3* direction, const s_real max_distance, const s_vec3* a, const s_vec3* b, const s_vec3* c, s_real* out_t, s_vec3* out_normal) {
	if (!origin || !direction || !a || !b || !c || max_distance <= 0.0f) {
		return false;
	}
	const s_vec3 edge1 = s_vec3(b->x - a->x, b->y - a->y, b->z - a->z);
	const s_vec3 edge2 = s_vec3(c->x - a->x, c->y - a->y, c->z - a->z);
	const s_vec3 pvec = s_vec3_cross(direction, &edge2);
	const s_real det = s_vec3_dot(&edge1, &pvec);
	if (s_real_abs(det) < S_EPSILON) {
		return false;
	}
	const s_real inv_det = 1.0f / det;
	const s_vec3 tvec = s_vec3(origin->x - a->x, origin->y - a->y, origin->z - a->z);
	const s_real u = s_vec3_dot(&tvec, &pvec) * inv_det;
	if (u < 0.0f || u > 1.0f) {
		return false;
	}
	const s_vec3 qvec = s_vec3_cross(&tvec, &edge1);
	const s_real v = s_vec3_dot(direction, &qvec) * inv_det;
	if (v < 0.0f || u + v > 1.0f) {
		return false;
	}
	const s_real t = s_vec3_dot(&edge2, &qvec) * inv_det;
	if (t < 0.0f || t > max_distance) {
		return false;
	}
	if (out_t) {
		*out_t = t;
	}
	if (out_normal) {
		const s_vec3 n = s_vec3_cross(&edge1, &edge2);
		const s_real len = s_vec3_length(&n);
		*out_normal = len < S_EPSILON ? s_vec3(0.0f, 1.0f, 0.0f) : s_vec3_divs(&n, len);
	}
	return true;
}

static inline s_vec2 s_closest_point_on_segment_2d(const s_vec2* a, const s_vec2* b, const s_vec2* p) {
	if (!a || !b || !p) {
		return s_vec2(0.0f, 0.0f);
	}
	const s_vec2 ab = s_vec2_sub(b, a);
	const s_real denom = s_vec2_dot(&ab, &ab);
	s_real t = 0.0f;
	if (denom > S_EPSILON) {
		const s_vec2 ap = s_vec2_sub(p, a);
		t = s_vec2_dot(&ap, &ab) / denom;
	}
	t = s_clamp01(t);
	return s_vec2(a->x + ab.x * t, a->y + ab.y * t);
}

static inline s_vec3 s_closest_point_on_segment_3d(const s_vec3* a, const s_vec3* b, const s_vec3* p) {
	if (!a || !b || !p) {
		return s_vec3(0.0f, 0.0f, 0.0f);
	}
	const s_vec3 ab = s_vec3_sub(b, a);
	const s_real denom = s_vec3_dot(&ab, &ab);
	s_real t = 0.0f;
	if (denom > S_EPSILON) {
		const s_vec3 ap = s_vec3_sub(p, a);
		t = s_vec3_dot(&ap, &ab) / denom;
	}
	t = s_clamp01(t);
	return s_vec3(a->x + ab.x * t, a->y + ab.y * t, a->z + ab.z * t);
}

static inline s_vec2 s_closest_point_on_triangle_2d(const s_vec2* a, const s_vec2* b, const s_vec2* c, const s_vec2* p) {
	if (!a || !b || !c || !p) {
		return s_vec2(0.0f, 0.0f);
	}
	const s_vec2 ab = s_vec2_sub(b, a);
	const s_vec2 ac = s_vec2_sub(c, a);
	const s_vec2 ap = s_vec2_sub(p, a);
	const s_real d1 = s_vec2_dot(&ab, &ap);
	const s_real d2 = s_vec2_dot(&ac, &ap);
	if (d1 <= 0.0f && d2 <= 0.0f) {
		return *a;
	}
	const s_vec2 bp = s_vec2_sub(p, b);
	const s_real d3 = s_vec2_dot(&ab, &bp);
	const s_real d4 = s_vec2_dot(&ac, &bp);
	if (d3 >= 0.0f && d4 <= d3) {
		return *b;
	}
	const s_real vc = (d1 * d4) - (d3 * d2);
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		const s_real v = d1 / (d1 - d3);
		return s_vec2(a->x + ab.x * v, a->y + ab.y * v);
	}
	const s_vec2 cp = s_vec2_sub(p, c);
	const s_real d5 = s_vec2_dot(&ab, &cp);
	const s_real d6 = s_vec2_dot(&ac, &cp);
	if (d6 >= 0.0f && d5 <= d6) {
		return *c;
	}
	const s_real vb = (d5 * d2) - (d1 * d6);
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		const s_real w = d2 / (d2 - d6);
		return s_vec2(a->x + ac.x * w, a->y + ac.y * w);
	}
	const s_real va = (d3 * d6) - (d5 * d4);
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		const s_vec2 bc = s_vec2_sub(c, b);
		const s_real w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return s_vec2(b->x + bc.x * w, b->y + bc.y * w);
	}
	const s_real denom = 1.0f / (va + vb + vc);
	const s_real v = vb * denom;
	const s_real w = vc * denom;
	return s_vec2(a->x + ab.x * v + ac.x * w, a->y + ab.y * v + ac.y * w);
}

static inline s_vec3 s_closest_point_on_triangle_3d(const s_vec3* a, const s_vec3* b, const s_vec3* c, const s_vec3* p) {
	if (!a || !b || !c || !p) {
		return s_vec3(0.0f, 0.0f, 0.0f);
	}
	const s_vec3 ab = s_vec3_sub(b, a);
	const s_vec3 ac = s_vec3_sub(c, a);
	const s_vec3 ap = s_vec3_sub(p, a);
	const s_real d1 = s_vec3_dot(&ab, &ap);
	const s_real d2 = s_vec3_dot(&ac, &ap);
	if (d1 <= 0.0f && d2 <= 0.0f) {
		return *a;
	}
	const s_vec3 bp = s_vec3_sub(p, b);
	const s_real d3 = s_vec3_dot(&ab, &bp);
	const s_real d4 = s_vec3_dot(&ac, &bp);
	if (d3 >= 0.0f && d4 <= d3) {
		return *b;
	}
	const s_real vc = (d1 * d4) - (d3 * d2);
	if (vc <= 0.0f && d1 >= 0.0f && d3 <= 0.0f) {
		const s_real v = d1 / (d1 - d3);
		return s_vec3(a->x + ab.x * v, a->y + ab.y * v, a->z + ab.z * v);
	}
	const s_vec3 cp = s_vec3_sub(p, c);
	const s_real d5 = s_vec3_dot(&ab, &cp);
	const s_real d6 = s_vec3_dot(&ac, &cp);
	if (d6 >= 0.0f && d5 <= d6) {
		return *c;
	}
	const s_real vb = (d5 * d2) - (d1 * d6);
	if (vb <= 0.0f && d2 >= 0.0f && d6 <= 0.0f) {
		const s_real w = d2 / (d2 - d6);
		return s_vec3(a->x + ac.x * w, a->y + ac.y * w, a->z + ac.z * w);
	}
	const s_real va = (d3 * d6) - (d5 * d4);
	if (va <= 0.0f && (d4 - d3) >= 0.0f && (d5 - d6) >= 0.0f) {
		const s_vec3 bc = s_vec3_sub(c, b);
		const s_real w = (d4 - d3) / ((d4 - d3) + (d5 - d6));
		return s_vec3(b->x + bc.x * w, b->y + bc.y * w, b->z + bc.z * w);
	}
	const s_real denom = 1.0f / (va + vb + vc);
	const s_real v = vb * denom;
	const s_real w = vc * denom;
	return s_vec3(a->x + ab.x * v + ac.x * w, a->y + ab.y * v + ac.y * w, a->z + ab.z * v + ac.z * w);
}

static inline s_vec2 s_triangle_centroid_2d(const s_vec2* a, const s_vec2* b, const s_vec2* c) {
	if (!a || !b || !c) {
		return s_vec2(0.0f, 0.0f);
	}
	return s_vec2((a->x + b->x + c->x) / 3.0f, (a->y + b->y + c->y) / 3.0f);
}

static inline s_vec3 s_triangle_centroid_3d(const s_vec3* a, const s_vec3* b, const s_vec3* c) {
	if (!a || !b || !c) {
		return s_vec3(0.0f, 0.0f, 0.0f);
	}
	return s_vec3((a->x + b->x + c->x) / 3.0f, (a->y + b->y + c->y) / 3.0f, (a->z + b->z + c->z) / 3.0f);
}

static inline s_box_2d s_triangle_bounds_2d(const s_vec2* a, const s_vec2* b, const s_vec2* c) {
	if (!a || !b || !c) {
		return s_box_2d_from_edges(0.0f, 0.0f, 0.0f, 0.0f);
	}
	s_box_2d bounds = s_box_2d_empty();
	s_box_2d_add_point(&bounds, a);
	s_box_2d_add_point(&bounds, b);
	s_box_2d_add_point(&bounds, c);
	return bounds;
}

static inline s_box_3d s_triangle_bounds_3d(const s_vec3* a, const s_vec3* b, const s_vec3* c) {
	if (!a || !b || !c) {
		return s_box_3d_from_edges(0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f);
	}
	s_box_3d bounds = s_box_3d_empty();
	s_box_3d_add_point(&bounds, a);
	s_box_3d_add_point(&bounds, b);
	s_box_3d_add_point(&bounds, c);
	return bounds;
}

#endif // S_MATH_H
