# Syphax
Single-header utility library for C.

Features:
- Types and math (vectors, matrices)
- Static arrays
- Files and paths
- JSON parser/writer
- Threads, mutexes, conditions

Examples:
- examples/types.c
- examples/array.c
- examples/files.c
- examples/json.c
- examples/thread.c

Snippets:

```c
#include "s_array.h"

int main(void) {
    s_array(int, values);
    s_array_init(&values, 4);
    s_array_add(&values, 10);
    s_array_add(&values, 20);
    s_array_clear(&values);
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

License: MIT
