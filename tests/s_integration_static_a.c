/**
 * Syphax - Ougi Washi
 *
 * Static multi-translation-unit integration test, TU A.
 */

#define SYPHAX_STATIC
#include "../s_files.h"
#include "../s_json.h"
#include "../s_memory.h"
#include "../s_thread.h"

b8 s_integration_static_files(char* out_path, sz out_cap) {
    if (!s_path_join(out_path, out_cap, "tests", "s_integration_static.tmp")) return false;
    if (!s_file_write(out_path, "static", 6)) return false;
    char* text = NULL;
    sz size = 0;
    if (!s_file_read(out_path, &text, &size)) return false;
    const b8 ok = size == 6 && text[0] == 's' && text[5] == 'c';
    s_free(text);
    return ok;
}

void* s_integration_static_thread_fn(void* arg) {
    i32* value = (i32*)arg;
    if (value != NULL) *value += 1;
    return value;
}
