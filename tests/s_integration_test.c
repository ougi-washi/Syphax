/**
 * Syphax - Ougi Washi
 *
 * Integration tests for allocator overrides and multi-header embedding.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_alloc_count = 0;
static int g_fail_at = -1;

static void* test_malloc(size_t size) {
    if (g_fail_at >= 0 && g_alloc_count++ >= g_fail_at) return NULL;
    return malloc(size);
}

static void* test_realloc(void* ptr, size_t size) {
    if (g_fail_at >= 0 && g_alloc_count++ >= g_fail_at) return NULL;
    return realloc(ptr, size);
}

static void test_free(void* ptr) {
    free(ptr);
}

#define SYPHAX_MALLOC test_malloc
#define SYPHAX_REALLOC test_realloc
#define SYPHAX_FREE test_free
#define SYPHAX_STATIC

#include "../s_array.h"
#include "../s_files.h"
#include "../s_json.h"
#include "../s_math.h"
#include "../s_memory.h"
#include "../s_thread.h"

static void require_true(b8 cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

static void reset_allocator(int fail_at) {
    g_alloc_count = 0;
    g_fail_at = fail_at;
}

typedef b8 (*oom_case_fn)(int fail_at);

static void require_oom_sweep(oom_case_fn fn, const char* msg) {
    for (int fail_at = 0; fail_at < 128; ++fail_at) {
        if (fn(fail_at)) return;
    }
    require_true(false, msg);
}

static b8 oom_array_reserve_case(int fail_at) {
    reset_allocator(-1);
    s_array(int, arr);
    s_array_init(&arr);
    int value = 5;
    s_handle h = s_array_add(&arr, value);
    require_true(h != S_HANDLE_NULL, "array reserve OOM setup");

    reset_allocator(fail_at);
    b8 ok = s_array_reserve(&arr, 64);
    reset_allocator(-1);
    require_true(s_array_get_size(&arr) == 1, "array reserve OOM keeps size");
    require_true(*(int*)s_array_get(&arr, h) == 5, "array reserve OOM keeps data");
    s_array_clear(&arr);
    return ok;
}

static b8 oom_array_append_case(int fail_at) {
    reset_allocator(-1);
    s_array(int, arr);
    s_array_init(&arr);
    int value = 7;
    s_handle h = s_array_add(&arr, value);
    int values[4] = { 1, 2, 3, 4 };
    require_true(h != S_HANDLE_NULL, "array append OOM setup");

    reset_allocator(fail_at);
    b8 ok = s_array_append_many(&arr, values, 4) != S_HANDLE_NULL;
    reset_allocator(-1);
    if (!ok) {
        require_true(s_array_get_size(&arr) == 1, "array append OOM keeps size");
        require_true(*(int*)s_array_get(&arr, h) == 7, "array append OOM keeps data");
    } else {
        require_true(s_array_get_size(&arr) == 5, "array append OOM sweep reaches success");
    }
    s_array_clear(&arr);
    return ok;
}

static b8 oom_array_copy_case(int fail_at) {
    reset_allocator(-1);
    s_array(int, src);
    s_array(int, dst);
    s_array_init(&src);
    s_array_init(&dst);
    int src_values[3] = { 1, 2, 3 };
    int old = 99;
    require_true(s_array_append_many(&src, src_values, 3) != S_HANDLE_NULL, "array copy OOM source setup");
    require_true(s_array_add(&dst, old) != S_HANDLE_NULL, "array copy OOM dst setup");

    reset_allocator(fail_at);
    b8 ok = s_array_copy(&dst, &src);
    reset_allocator(-1);
    if (!ok) {
        require_true(s_array_get_size(&dst) == 1, "array copy OOM keeps dst size");
        require_true(*(int*)s_array_get(&dst, s_array_handle(&dst, 0)) == 99, "array copy OOM keeps dst data");
    } else {
        require_true(s_array_get_size(&dst) == 3, "array copy OOM sweep reaches success");
        require_true(*(int*)s_array_get(&dst, s_array_handle(&dst, 2)) == 3, "array copy success data");
    }
    s_array_clear(&src);
    s_array_clear(&dst);
    return ok;
}

static b8 oom_json_parse_case(int fail_at) {
    reset_allocator(fail_at);
    s_json_error err = {0};
    s_json* root = s_json_parse_with_error("{\"a\":[1,\"x\",true],\"b\":{\"c\":\"d\"}}", &err);
    reset_allocator(-1);
    if (root != NULL) {
        s_json_free(root);
        return true;
    }
    require_true(err.code == S_JSON_ERR_OOM, "json parse OOM reports OOM");
    return false;
}

static b8 oom_json_stringify_case(int fail_at) {
    reset_allocator(-1);
    s_json* root = s_json_parse("{\"a\":[1,\"x\",true],\"b\":{\"c\":\"d\"}}");
    require_true(root != NULL, "json stringify OOM setup");

    reset_allocator(fail_at);
    char* text = s_json_stringify(root);
    reset_allocator(-1);
    if (text != NULL) {
        s_free(text);
        s_json_free(root);
        return true;
    }
    text = s_json_stringify(root);
    require_true(text != NULL, "json stringify OOM leaves tree usable");
    s_free(text);
    s_json_free(root);
    return false;
}

int main(void) {
    s_array(int, arr);
    s_array_init(&arr);

    int value = 7;
    for (int fail_at = 0; fail_at < 4; ++fail_at) {
        reset_allocator(fail_at);
        require_true(s_array_add(&arr, value) == S_HANDLE_NULL, "array add fails at allocation point");
        require_true(s_array_get_size(&arr) == 0, "array OOM keeps size");
        reset_allocator(-1);
        s_array_clear(&arr);
        s_array_init(&arr);
    }

    reset_allocator(-1);
    s_handle h = s_array_add(&arr, value);
    require_true(h != S_HANDLE_NULL, "array add recovers after OOM");
    require_true(s_array_get_size(&arr) == 1, "array size after recovery");

    reset_allocator(1);
    int more[32] = {0};
    require_true(s_array_append_many(&arr, more, 32) == S_HANDLE_NULL, "array append_many fails cleanly");
    require_true(s_array_get_size(&arr) == 1, "array append_many OOM keeps old size");
    require_true(*(int*)s_array_get(&arr, h) == 7, "array append_many OOM keeps old data");
    s_array_clear(&arr);
    require_oom_sweep(oom_array_reserve_case, "array reserve OOM sweep reaches success");
    require_oom_sweep(oom_array_append_case, "array append OOM sweep reaches success");
    require_oom_sweep(oom_array_copy_case, "array copy OOM sweep reaches success");

    reset_allocator(0);
    require_true(s_json_str("name", "value") == NULL, "json node OOM returns NULL");
    s_json_error err = {0};
    require_true(s_json_parse_with_error("true", &err) == NULL && err.code == S_JSON_ERR_OOM, "json literal OOM reports error");
    err.code = S_JSON_OK;
    require_true(s_json_parse_with_error("\"\"", &err) == NULL && err.code == S_JSON_ERR_OOM, "json empty string OOM reports error");
    require_oom_sweep(oom_json_parse_case, "json parse OOM sweep reaches success");
    require_oom_sweep(oom_json_stringify_case, "json stringify OOM sweep reaches success");

    reset_allocator(-1);
    s_json* json = s_json_object_empty(NULL);
    require_true(json != NULL, "json object after OOM recovers");
    reset_allocator(0);
    require_true(!s_json_object_add_str(json, "key", "value"), "json add OOM returns false");
    require_true(s_json_count(json) == 0, "json add OOM keeps object unchanged");
    reset_allocator(-1);
    s_json_free(json);

    json = s_json_object_empty(NULL);
    s_json* child = s_json_str("old", "value");
    require_true(json != NULL && child != NULL, "json direct add setup");
    reset_allocator(1);
    require_true(!s_json_object_add(json, "new", child), "json object_add reserve OOM returns false");
    require_true(s_json_count(json) == 0, "json object_add reserve OOM keeps object empty");
    require_true(child->name != NULL && strcmp(child->name, "old") == 0, "json object_add reserve OOM keeps child name");
    reset_allocator(0);
    s_json_set_name(child, "new");
    require_true(child->name != NULL && strcmp(child->name, "old") == 0, "json set_name OOM keeps old name");
    reset_allocator(-1);
    s_json_free(child);
    s_json_free(json);

    s_memory_arena arena = {0};
    require_true(s_memory_arena_init(&arena, 256), "memory arena uses allocator override");
    void* p = s_memory_malloc(&arena, 32);
    require_true(p != NULL, "memory arena allocation");
    require_true(s_memory_free_debug(&arena, p), "memory arena free");
    s_memory_arena_release(&arena);

    printf("All integration tests passed.\n");
    return 0;
}
