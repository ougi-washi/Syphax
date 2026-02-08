#include "../s_memory.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    s_memory_arena arena = {0};
    s_memory_arena_init(&arena, 1024);

    const char* msg = "hello";
    char* buf = (char*)s_malloc(&arena, 32);
    if (buf == NULL) return 1;
    memcpy(buf, msg, strlen(msg) + 1);
    printf("buf=%s\n", buf);

    char* grow = (char*)s_realloc(&arena, buf, 64);
    if (grow == NULL) return 1;
    strcat(grow, " world");
    printf("grow=%s\n", grow);

    s_free(&arena, grow);
    s_memory_arena_report_leaks(&arena, NULL);
    s_memory_arena_release(&arena);
    return 0;
}
