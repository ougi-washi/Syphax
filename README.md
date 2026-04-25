# Syphax
Single-header utility library for C.

Features:
- Types and math (vectors, matrices)
- Static arrays
- Files and paths
- JSON parser/writer
- JSON typed getters, object/array helpers, and simple paths
- Threads, mutexes, conditions
- Memory arena with debug tracking

Examples:
- examples/types.c
- examples/array.c
- examples/files.c
- examples/json.c
- examples/thread.c
- examples/memory.c

Tests:
- tests/s_files_test.c
- tests/s_thread_test.c
- tests/s_memory_test.c

Snippets:

```c
#include "s_array.h"
#include <stdio.h>

int main(void) {
    s_array(int, scores);

    int a = 7;
    int b = 12;
    s_handle ha = s_array_add(&scores, a);
    s_handle hb = s_array_add(&scores, b);

    int* pa = s_array_get(&scores, ha);
    if (pa) *pa = 8;

    for (sz i = 0; i < s_array_get_size(&scores); ++i) {
        s_handle hi = s_array_handle(&scores, (u32)i);
        int* item = s_array_get(&scores, hi);
        if (item) printf("score[%zu] = %d\n", i, *item);
    }

    s_array_clear(&scores);
    return 0;
}
```

```c
#include "s_json.h"
#include <stdlib.h>

int main(void) {
    s_json* doc = s_json_object("root",
        s_json_str("name", "Syphax"),
        s_json_int("build", 1)
    );
    char* text = s_json_stringify(doc);
    s_json_free(doc);
    free(text);
    return 0;
}
```

```c
#include "s_files.h"

int main(void) {
    s_file_write("out.txt", "hi", 2);
    s_file_remove("out.txt");
    return 0;
}
```

```c
#include "s_thread.h"

static void* work(void* arg) {
    (void)arg;
    return NULL;
}

int main(void) {
    s_thread t;
    s_thread_create(&t, work, NULL);
    s_thread_join(&t, NULL);
    return 0;
}
```

```c
#include "s_memory.h"

int main(void) {
    s_memory_arena arena = {0};
    s_memory_arena_init(&arena, 1024 * 1024);

    char* msg = (char*)s_malloc(&arena, 32);
    if (msg == NULL) return 1;
    s_free(&arena, msg);

    s_memory_arena_report_leaks(&arena, NULL);
    s_memory_arena_release(&arena);
    return 0;
}
```

License: MIT
