/**
 * Syphax - Ougi Washi
 *
 * Multi-translation-unit test for declaration/implementation mode.
 */

#include "../s_array.h"
#include "../s_files.h"
#include "../s_json.h"
#include "../s_math.h"
#include "../s_memory.h"
#include "../s_thread.h"
#include <stdio.h>
#include <stdlib.h>

static void require_true(b8 cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

int main(void) {
    char path[256];
    require_true(s_path_join(path, sizeof(path), "tests", "s_integration_impl.tmp"), "path join");
    require_true(s_file_write(path, "ok", 2), "file write");
    char* text = NULL;
    sz size = 0;
    require_true(s_file_read(path, &text, &size), "file read");
    require_true(size == 2 && text[0] == 'o' && text[1] == 'k', "file read content");
    s_free(text);
    require_true(s_file_remove(path), "file remove");

    s_json* json = s_json_object_empty(NULL);
    require_true(json != NULL, "json object");
    require_true(s_json_object_add_int(json, "n", 7), "json add int");
    char* out = s_json_stringify(json);
    require_true(out != NULL, "json stringify");
    s_free(out);
    s_json_free(json);

    s_memory_arena arena = {0};
    require_true(s_memory_arena_init(&arena, 128), "memory init");
    void* p = s_memory_malloc(&arena, 16);
    require_true(p != NULL, "memory alloc");
    require_true(s_memory_free_debug(&arena, p), "memory free");
    s_memory_arena_release(&arena);

    s_array(int, arr);
    s_array_init(&arr);
    int v = 3;
    require_true(s_array_add(&arr, v) != S_HANDLE_NULL, "array add");
    s_array_clear(&arr);

    s_vec2 a = s_vec2(3.0f, 4.0f);
    require_true(s_precision_equal(s_vec2_length(&a), 5.0f, S_EPSILON), "math length");

    printf("Implementation integration test passed.\n");
    return 0;
}
