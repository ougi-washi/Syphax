#include "s_array.h"
#include <stdio.h>

int main(void) {
    s_array(int, numbers);
    s_array_init(&numbers);

    int v0 = 3;
    int v1 = 7;
    int v2 = 11;
    s_array_add(&numbers, v0);
    s_handle h1 = s_array_add(&numbers, v1);
    s_array_add(&numbers, v2);

    s_handle h3 = s_array_increment(&numbers);
    int* p3 = s_array_get(&numbers, h3);
    if (p3 != NULL) {
        *p3 = 42;
    }

    printf("size=%zu\n", s_array_get_size(&numbers));
    int* it = NULL;
    sz index = 0;
    s_foreach(&numbers, it) {
        printf("[%zu]=%d\n", index++, *it);
    }

    if (!s_array_remove(&numbers, h1)) {
        fprintf(stderr, "remove failed\n");
        s_array_clear(&numbers);
        return 1;
    }

    if (s_array_get(&numbers, h1) != NULL) {
        fprintf(stderr, "stale handle should be invalid\n");
        s_array_clear(&numbers);
        return 1;
    }

    printf("after_remove size=%zu\n", s_array_get_size(&numbers));
    index = 0;
    s_foreach(&numbers, it) {
        printf("[%zu]=%d\n", index++, *it);
    }

    s_array_clear(&numbers);
    return 0;
}
