# Syphax

Small dependency-free C99 utility headers.

Modules:
- `s_types.h`: core types, allocator/assert hooks
- `s_math.h`: vectors, matrices, geometry
- `s_array.h`: handle-based dynamic arrays
- `s_files.h`: file/path helpers
- `s_json.h`: strict JSON reader/writer
- `s_thread.h`: threads, mutexes, conditions
- `s_memory.h`: arena/debug allocator

## Integration

Tiny helpers in `s_types.h`, `s_math.h`, and `s_array.h` are header-only.

Heavier modules support two modes:

```c
/* One implementation translation unit. */
#define SYPHAX_IMPLEMENTATION
#include "s_files.h"
#include "s_json.h"
#include "s_memory.h"
#include "s_thread.h"
```

```c
/* Other translation units include declarations normally. */
#include "s_files.h"
#include "s_json.h"
#include "s_memory.h"
#include "s_thread.h"
```

For single-file embedding, define `SYPHAX_STATIC` before including a heavy
module.

Allocator and abort hooks:

```c
#define SYPHAX_MALLOC  my_malloc
#define SYPHAX_REALLOC my_realloc
#define SYPHAX_FREE    my_free
#define SYPHAX_ABORT   my_abort
#include "s_json.h"
```

Memory returned by Syphax APIs, such as `s_file_read` and `s_json_stringify`,
must be released with `s_free`.

`s_memory.h` is an arena/debug allocator. It tracks frees and leaks, but it is
not a reclaiming `malloc` replacement.

Strict C++ compatibility is not promised. The supported language is C99.

## Build

Build examples directly:

```sh
cc -std=c99 -Wall -Wextra -O2 -I. examples/types.c -o /tmp/syphax/types -lm
cc -std=c99 -Wall -Wextra -O2 -I. examples/array.c -o /tmp/syphax/array
cc -std=c99 -Wall -Wextra -O2 -I. examples/files.c -o /tmp/syphax/files
cc -std=c99 -Wall -Wextra -O2 -I. examples/json.c -o /tmp/syphax/json -lm
cc -std=c99 -Wall -Wextra -O2 -I. examples/memory.c -o /tmp/syphax/memory
cc -std=c99 -Wall -Wextra -O2 -I. examples/thread.c -o /tmp/syphax/thread -pthread
```

Build tests:

```sh
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_array_test.c -o /tmp/syphax/s_array_test
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_files_test.c -o /tmp/syphax/s_files_test
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_json_test.c -o /tmp/syphax/s_json_test -lm
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_math_test.c -o /tmp/syphax/s_math_test -lm
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_memory_test.c -o /tmp/syphax/s_memory_test
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_thread_test.c -o /tmp/syphax/s_thread_test -pthread
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_types_test.c -o /tmp/syphax/s_types_test -lm
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_integration_test.c -o /tmp/syphax/s_integration_test -pthread -lm
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_integration_impl.c tests/s_integration_use.c -o /tmp/syphax/s_integration_impl_test -pthread -lm
cc -std=c99 -Wall -Wextra -O2 -I. tests/s_integration_static_main.c tests/s_integration_static_a.c tests/s_integration_static_b.c -o /tmp/syphax/s_integration_static_test -pthread -lm
```

License: MIT
