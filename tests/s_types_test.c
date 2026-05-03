/**
 * Syphax - Ougi Washi
 *
 * Tests for s_types.h epsilon comparisons.
 */

#include "../s_math.h"
#include <stdio.h>
#include <stdlib.h>

static void require_true(b8 cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

static void require_i32(i32 actual, i32 expected, const char* msg) {
    if (actual != expected) {
        fprintf(stderr, "FAIL: %s: got %d expected %d\n", msg, actual, expected);
        exit(1);
    }
}

static void require_uvec2(s_uvec2 actual, u32 x, u32 y, const char* msg) {
    if (actual.x != x || actual.y != y) {
        fprintf(stderr, "FAIL: %s: got (%lu, %lu) expected (%lu, %lu)\n",
            msg, (unsigned long)actual.x, (unsigned long)actual.y,
            (unsigned long)x, (unsigned long)y);
        exit(1);
    }
}

static void require_uvec3(s_uvec3 actual, u32 x, u32 y, u32 z, const char* msg) {
    if (actual.x != x || actual.y != y || actual.z != z) {
        fprintf(stderr, "FAIL: %s: got (%lu, %lu, %lu) expected (%lu, %lu, %lu)\n",
            msg, (unsigned long)actual.x, (unsigned long)actual.y, (unsigned long)actual.z,
            (unsigned long)x, (unsigned long)y, (unsigned long)z);
        exit(1);
    }
}

static void require_uvec4(s_uvec4 actual, u32 x, u32 y, u32 z, u32 w, const char* msg) {
    if (actual.x != x || actual.y != y || actual.z != z || actual.w != w) {
        fprintf(stderr, "FAIL: %s: got (%lu, %lu, %lu, %lu) expected (%lu, %lu, %lu, %lu)\n",
            msg, (unsigned long)actual.x, (unsigned long)actual.y,
            (unsigned long)actual.z, (unsigned long)actual.w,
            (unsigned long)x, (unsigned long)y, (unsigned long)z, (unsigned long)w);
        exit(1);
    }
}

int main(void) {
    const s_real eps_wide = (s_real)0.0001f;
    const s_real eps_tight = (s_real)0.00001f;

    require_true(s_f32_equal(1.0f, 1.0f + 0.00009f, 0.0001f), "s_f32_equal in range");
    require_true(!s_f32_equal(1.0f, 1.0f + 0.001f, 0.0001f), "s_f32_equal out of range");

    require_true(s_f64_equal(1.0, 1.0 + 0.0000009, 0.000001), "s_f64_equal in range");
    require_true(!s_f64_equal(1.0, 1.0 + 0.0001, 0.000001), "s_f64_equal out of range");


    s_vec2 v2a = s_vec2(1.0f, -2.0f);
    s_vec2 v2b = s_vec2(1.0f + 0.00005f, -2.0f - 0.00005f);
    s_vec2 v2abs = s_vec2_abs(&v2a);
    require_true(s_vec2_equal(&v2abs, &(s_vec2(1.0f, 2.0f)), eps_tight), "s_vec2_abs");
    require_true(s_vec2_equal(&v2a, &v2b, eps_wide), "s_vec2_equal in range");
    require_true(!s_vec2_equal(&v2a, &v2b, eps_tight), "s_vec2_equal out of range");

    s_vec3 v3a = s_vec3(1.0f, 2.0f, 3.0f);
    s_vec3 v3b = s_vec3(1.0f + 0.00004f, 2.0f - 0.00004f, 3.0f + 0.00004f);
    s_vec3 v3neg = s_vec3(-1.0f, 2.0f, -3.0f);
    s_vec3 v3abs = s_vec3_abs(&v3neg);
    require_true(s_vec3_equal(&v3abs, &v3a, eps_tight), "s_vec3_abs");
    require_true(s_vec3_equal(&v3a, &v3b, eps_wide), "s_vec3_equal in range");
    require_true(!s_vec3_equal(&v3a, &v3b, eps_tight), "s_vec3_equal out of range");

    s_vec4 v4a = s_vec4(1.0f, 2.0f, 3.0f, 4.0f);
    s_vec4 v4b = s_vec4(1.0f - 0.00004f, 2.0f + 0.00004f, 3.0f - 0.00004f, 4.0f + 0.00004f);
    s_vec4 v4neg = s_vec4(-1.0f, 2.0f, -3.0f, 4.0f);
    s_vec4 v4abs = s_vec4_abs(&v4neg);
    require_true(s_vec4_equal(&v4abs, &v4a, eps_tight), "s_vec4_abs");
    require_true(s_vec4_equal(&v4a, &v4b, eps_wide), "s_vec4_equal in range");
    require_true(!s_vec4_equal(&v4a, &v4b, eps_tight), "s_vec4_equal out of range");

    require_i32(s_i32_abs(INT32_MIN), INT32_MAX, "s_i32_abs saturates INT32_MIN");
    s_ivec2 i2a = s_ivec2(3, 5);
    s_ivec2 i2b = s_ivec2(2, -2);
    s_ivec2 i2abs = s_ivec2_abs(&i2b);
    require_i32(i2abs.x, 2, "s_ivec2_abs x");
    require_i32(i2abs.y, 2, "s_ivec2_abs y");
    s_ivec2 i2 = s_ivec2_add(&i2a, &i2b);
    require_i32(i2.x, 5, "s_ivec2_add x");
    require_i32(i2.y, 3, "s_ivec2_add y");
    i2 = s_ivec2_sub(&i2a, &i2b);
    require_i32(i2.x, 1, "s_ivec2_sub x");
    require_i32(i2.y, 7, "s_ivec2_sub y");
    i2 = s_ivec2_mul(&i2a, &i2b);
    require_i32(i2.x, 6, "s_ivec2_mul x");
    require_i32(i2.y, -10, "s_ivec2_mul y");
    i2 = s_ivec2_div(&i2a, &i2b);
    require_i32(i2.x, 1, "s_ivec2_div x");
    require_i32(i2.y, -2, "s_ivec2_div y");

    s_ivec3 i3a = s_ivec3(-3, 3, -1);
    s_ivec3 i3b = s_ivec3(2, -2, 2);
    s_ivec3 i3abs = s_ivec3_abs(&i3a);
    require_i32(i3abs.x, 3, "s_ivec3_abs x");
    require_i32(i3abs.y, 3, "s_ivec3_abs y");
    require_i32(i3abs.z, 1, "s_ivec3_abs z");
    s_ivec3 i3 = s_ivec3_add(&i3a, &i3b);
    require_i32(i3.x, -1, "s_ivec3_add x");
    require_i32(i3.y, 1, "s_ivec3_add y");
    require_i32(i3.z, 1, "s_ivec3_add z");
    i3 = s_ivec3_sub(&i3a, &i3b);
    require_i32(i3.x, -5, "s_ivec3_sub x");
    require_i32(i3.y, 5, "s_ivec3_sub y");
    require_i32(i3.z, -3, "s_ivec3_sub z");
    i3 = s_ivec3_mul(&i3a, &i3b);
    require_i32(i3.x, -6, "s_ivec3_mul x");
    require_i32(i3.y, -6, "s_ivec3_mul y");
    require_i32(i3.z, -2, "s_ivec3_mul z");
    i3 = s_ivec3_div(&i3a, &i3b);
    require_i32(i3.x, -1, "s_ivec3_div x");
    require_i32(i3.y, -1, "s_ivec3_div y");
    require_i32(i3.z, 0, "s_ivec3_div z");

    s_ivec4 i4a = s_ivec4(7, -7, 8, -8);
    s_ivec4 i4b = s_ivec4(3, 3, -3, -3);
    s_ivec4 i4abs = s_ivec4_abs(&i4a);
    require_i32(i4abs.x, 7, "s_ivec4_abs x");
    require_i32(i4abs.y, 7, "s_ivec4_abs y");
    require_i32(i4abs.z, 8, "s_ivec4_abs z");
    require_i32(i4abs.w, 8, "s_ivec4_abs w");
    s_ivec4 i4 = s_ivec4_add(&i4a, &i4b);
    require_i32(i4.x, 10, "s_ivec4_add x");
    require_i32(i4.y, -4, "s_ivec4_add y");
    require_i32(i4.z, 5, "s_ivec4_add z");
    require_i32(i4.w, -11, "s_ivec4_add w");
    i4 = s_ivec4_sub(&i4a, &i4b);
    require_i32(i4.x, 4, "s_ivec4_sub x");
    require_i32(i4.y, -10, "s_ivec4_sub y");
    require_i32(i4.z, 11, "s_ivec4_sub z");
    require_i32(i4.w, -5, "s_ivec4_sub w");
    i4 = s_ivec4_mul(&i4a, &i4b);
    require_i32(i4.x, 21, "s_ivec4_mul x");
    require_i32(i4.y, -21, "s_ivec4_mul y");
    require_i32(i4.z, -24, "s_ivec4_mul z");
    require_i32(i4.w, 24, "s_ivec4_mul w");
    i4 = s_ivec4_div(&i4a, &i4b);
    require_i32(i4.x, 2, "s_ivec4_div x");
    require_i32(i4.y, -2, "s_ivec4_div y");
    require_i32(i4.z, -2, "s_ivec4_div z");
    require_i32(i4.w, 2, "s_ivec4_div w");

    s_uvec2 u2a = s_uvec2(3u, 5u);
    s_uvec2 u2b = s_uvec2(2u, 4u);
    require_uvec2(s_uvec2_add(&u2a, &u2b), 5u, 9u, "s_uvec2_add");
    require_uvec2(s_uvec2_sub(&u2b, &u2a), UINT32_MAX, UINT32_MAX, "s_uvec2_sub wraps");
    require_uvec2(s_uvec2_mul(&u2a, &u2b), 6u, 20u, "s_uvec2_mul");
    require_uvec2(s_uvec2_div(&u2a, &u2b), 1u, 1u, "s_uvec2_div");

    s_uvec3 u3a = s_uvec3(9u, 10u, 11u);
    s_uvec3 u3b = s_uvec3(3u, 5u, 2u);
    require_uvec3(s_uvec3_add(&u3a, &u3b), 12u, 15u, 13u, "s_uvec3_add");
    require_uvec3(s_uvec3_sub(&u3b, &u3a),
        UINT32_MAX - 5u, UINT32_MAX - 4u, UINT32_MAX - 8u, "s_uvec3_sub wraps");
    require_uvec3(s_uvec3_mul(&u3a, &u3b), 27u, 50u, 22u, "s_uvec3_mul");
    require_uvec3(s_uvec3_div(&u3a, &u3b), 3u, 2u, 5u, "s_uvec3_div");

    s_uvec4 u4a = s_uvec4(8u, 18u, 21u, 24u);
    s_uvec4 u4b = s_uvec4(4u, 3u, 7u, 6u);
    require_uvec4(s_uvec4_add(&u4a, &u4b), 12u, 21u, 28u, 30u, "s_uvec4_add");
    require_uvec4(s_uvec4_sub(&u4b, &u4a),
        UINT32_MAX - 3u, UINT32_MAX - 14u, UINT32_MAX - 13u, UINT32_MAX - 17u,
        "s_uvec4_sub wraps");
    require_uvec4(s_uvec4_mul(&u4a, &u4b), 32u, 54u, 147u, 144u, "s_uvec4_mul");
    require_uvec4(s_uvec4_div(&u4a, &u4b), 2u, 6u, 3u, 4u, "s_uvec4_div");

    s_mat3 m3a = s_mat3_identity;
    s_mat3 m3b = s_mat3_identity;
    m3b.m[1][2] = m3b.m[1][2] + 0.00005f;
    require_true(s_mat3_equal(&m3a, &m3b, eps_wide), "s_mat3_equal in range");
    require_true(!s_mat3_equal(&m3a, &m3b, eps_tight), "s_mat3_equal out of range");

    s_mat4 m4a = s_mat4_identity;
    s_mat4 m4b = s_mat4_identity;
    m4b.m[2][3] = m4b.m[2][3] - 0.00005f;
    require_true(s_mat4_equal(&m4a, &m4b, eps_wide), "s_mat4_equal in range");
    require_true(!s_mat4_equal(&m4a, &m4b, eps_tight), "s_mat4_equal out of range");

    printf("All s_types tests passed.\n");
    return 0;
}
