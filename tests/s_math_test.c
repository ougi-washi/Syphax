/**
 * Syphax - Ougi Washi
 *
 * Tests for s_math.h production edge cases.
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

static void require_real(s_real actual, s_real expected, const char* msg) {
    if (!s_precision_equal(actual, expected, (s_real)0.0001)) {
        fprintf(stderr, "FAIL: %s got %.8g expected %.8g\n", msg, (double)actual, (double)expected);
        exit(1);
    }
}

int main(void) {
    s_box_2d empty2 = s_box_2d_empty();
    require_true(s_box_2d_is_empty(&empty2), "2d empty sentinel is empty");
    s_box_2d transformed2 = s_box_2d_transform(&empty2, &(s_mat3_identity));
    require_true(s_box_2d_is_empty(&transformed2), "2d empty transform remains empty");
    s_box_2d valid2 = s_box_2d_from_edges(0.0f, 0.0f, 2.0f, 3.0f);
    s_box_2d_add(&valid2, &empty2);
    require_real(s_box_2d_width(&valid2), 2.0f, "2d empty add keeps width");
    require_real(s_box_2d_height(&valid2), 3.0f, "2d empty add keeps height");
    s_box_2d hit2 = s_box_2d_intersection(&valid2, &empty2);
    require_true(s_box_2d_is_empty(&hit2), "2d empty intersection");

    s_box_3d empty3 = s_box_3d_empty();
    require_true(s_box_3d_is_empty(&empty3), "3d empty sentinel is empty");
    s_box_3d transformed3 = s_box_3d_transform(&empty3, &(s_mat4_identity));
    require_true(s_box_3d_is_empty(&transformed3), "3d empty transform remains empty");
    s_box_3d valid3 = s_box_3d_from_edges(0.0f, 0.0f, 0.0f, 2.0f, 3.0f, 4.0f);
    s_box_3d_add(&valid3, &empty3);
    require_real(s_box_3d_width(&valid3), 2.0f, "3d empty add keeps width");
    require_real(s_box_3d_depth(&valid3), 4.0f, "3d empty add keeps depth");
    s_box_3d hit3 = s_box_3d_intersection(&valid3, &empty3);
    require_true(s_box_3d_is_empty(&hit3), "3d empty intersection");

    s_mat3 pivot3 = s_mat3(
        0.0f, 1.0f, 0.0f,
        1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f
    );
    s_mat3 inv3 = {0};
    require_true(s_mat3_inverse_checked(&pivot3, &inv3), "mat3 checked inverse pivots");
    s_mat3 product3 = s_mat3_mul(&pivot3, &inv3);
    require_true(s_mat3_equal(&product3, &(s_mat3_identity), (s_real)0.0001), "mat3 inverse product identity");

    s_mat4 pivot4 = s_mat4(
        0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    s_mat4 inv4 = {0};
    require_true(s_mat4_inverse_checked(&pivot4, &inv4), "mat4 checked inverse pivots");
    s_mat4 product4 = s_mat4_mul(&pivot4, &inv4);
    require_true(s_mat4_equal(&product4, &(s_mat4_identity), (s_real)0.0001), "mat4 inverse product identity");

    s_mat4 singular = s_mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f
    );
    require_true(!s_mat4_inverse_checked(&singular, &inv4), "singular mat4 inverse fails");

    printf("All s_math tests passed.\n");
    return 0;
}
