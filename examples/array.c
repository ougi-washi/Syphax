#include "s_array.h"
#include <stdio.h>

int main(void) {
    s_array(int, numbers);
    s_array_init(&numbers, 8);

    s_array_add(&numbers, 3);
    s_array_add(&numbers, 7);
    s_array_add(&numbers, 11);
    *s_array_increment(&numbers) = 42;

    printf("size=%zu\n", s_array_get_size(&numbers));
    s_foreach(&numbers, i) {
        printf("[%zu]=%d\n", i, *s_array_get(&numbers, i));
    }

    s_array_remove_at(&numbers, 1);

    printf("after_remove size=%zu\n", s_array_get_size(&numbers));
    s_foreach(&numbers, i) {
        printf("[%zu]=%d\n", i, *s_array_get(&numbers, i));
    }

    s_array_clear(&numbers);
    return 0;
}
