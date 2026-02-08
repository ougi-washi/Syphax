#include "s_types.h"
#include <stdio.h>

int main(void) {
    s_vec2 a = s_vec2(3.0f, 4.0f);
    printf("len=%.2f\n", s_vec2_length(&a));

    s_vec3 v = s_vec3(1.0f, 2.0f, 3.0f);
    s_vec3 n = s_vec3_normalize(&v);
    printf("n=(%.3f, %.3f, %.3f)\n", n.x, n.y, n.z);

    s_mat4 proj = s_mat4_perspective(3.14159265f / 3.0f, 16.0f / 9.0f, 0.1f, 100.0f);
    printf("proj00=%.3f\n", proj.m[0][0]);
    return 0;
}
