/**
 * Syphax - Ougi Washi
 *
 * Multi-translation-unit test for SYPHAX_STATIC mode.
 */

#include "../s_types.h"
#include <stdio.h>
#include <stdlib.h>

b8 s_integration_static_files(char* out_path, sz out_cap);
b8 s_integration_static_json_memory(void);
b8 s_integration_static_thread(void* (*fn)(void*));
void* s_integration_static_thread_fn(void* arg);

static void require_true(b8 cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

int main(void) {
    char path[256];
    require_true(s_integration_static_files(path, sizeof(path)), "static files TU");
    require_true(s_integration_static_json_memory(), "static json/memory TU");
    require_true(s_integration_static_thread(s_integration_static_thread_fn), "static thread TU");
    remove(path);
    printf("Static integration test passed.\n");
    return 0;
}
