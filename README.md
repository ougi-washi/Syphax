# Syphax
Single-header utility library for C.

Features:
- Types and math (vectors, matrices)
- Static arrays
- Files and paths
- JSON parser/writer
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

int main(void) {
    s_array(int, src);
    s_array(int, dst);
    
    s_array_init(&src, 4);
    s_array_init(&dst, 1);
    
    s_array_add(&src, 10);
    s_array_add(&src, 20);
    
    s_array_copy(&dst, &src);
    
    s_array_clear(&src);
    s_array_clear(&dst);
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
