/**
 * Syphax - Ougi Washi
 *
 * Static multi-translation-unit integration test, TU B.
 */

#define SYPHAX_STATIC
#include "../s_json.h"
#include "../s_memory.h"
#include "../s_thread.h"

b8 s_integration_static_json_memory(void) {
    s_json* root = s_json_object_empty(NULL);
    if (root == NULL) return false;
    if (!s_json_object_add_str(root, "mode", "static")) {
        s_json_free(root);
        return false;
    }
    char* text = s_json_stringify(root);
    if (text == NULL) {
        s_json_free(root);
        return false;
    }
    s_free(text);
    s_json_free(root);

    s_memory_arena arena = {0};
    if (!s_memory_arena_init(&arena, 128)) return false;
    void* ptr = s_memory_malloc(&arena, 16);
    const b8 ok = ptr != NULL && s_memory_free_debug(&arena, ptr);
    s_memory_arena_release(&arena);
    return ok;
}

b8 s_integration_static_thread(void* (*fn)(void*)) {
    if (fn == NULL) return false;
    s_thread thread;
    i32 value = 1;
    void* ret = NULL;
    if (!s_thread_create(&thread, fn, &value)) return false;
    if (!s_thread_join(&thread, &ret)) return false;
    return ret == &value && value == 2;
}
