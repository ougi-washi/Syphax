/**
 * Syphax - Ougi Washi
 *
 * Tests for s_memory.h
 */

#define SYPHAX_STATIC
#include "../s_memory.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void require_true(b8 cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

static void require_mem_eq(const void* a, const void* b, sz size, const char* msg) {
    if (size > 0 && memcmp(a, b, size) != 0) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

int main(void) {
    s_memory_arena arena = {0};
    require_true(s_memory_arena_init(&arena, 4096), "arena init returns true");
    require_true(arena.seg_head != NULL && arena.seg_head->buffer != NULL, "arena init");

    i32 line_a = __LINE__ + 1;
    u8* a = (u8*)s_memory_malloc(&arena, 16);
    require_true(a != NULL, "s_malloc a");
    require_true(s_memory_arena_active_count(&arena) == 1, "active count after a");
    require_true(s_memory_arena_active_bytes(&arena) == 16, "active bytes after a");
    require_true(s_memory_arena_bytes_for_line(&arena, __FILE__, line_a) == 16, "bytes for line a");

    i32 line_b = __LINE__ + 1;
    u8* b = (u8*)s_memory_calloc(&arena, 4, 8);
    require_true(b != NULL, "s_calloc b");
    for (sz i = 0; i < 32; i++) {
        require_true(b[i] == 0, "calloc zeroed");
    }
    require_true(s_memory_arena_bytes_for_line(&arena, __FILE__, line_b) == 32, "bytes for line b");

    i32 line_c = __LINE__ + 1;
    u8* c = (u8*)s_memory_malloc(&arena, 8);
    require_true(c != NULL, "s_malloc c");
    for (sz i = 0; i < 8; i++) c[i] = (u8)(i + 1);

    i32 line_r = __LINE__ + 1;
    u8* c2 = (u8*)s_memory_realloc_debug(&arena, c, 24);
    require_true(c2 != NULL, "s_realloc c");
    require_mem_eq(c2, (u8[]){1,2,3,4,5,6,7,8}, 8, "realloc preserves data");
    require_true(s_memory_arena_bytes_for_line(&arena, __FILE__, line_c) == 0, "old realloc line freed");
    require_true(s_memory_arena_bytes_for_line(&arena, __FILE__, line_r) == 24, "realloc line bytes");

    require_true(s_memory_free_debug(&arena, a), "s_free a");
    require_true(!s_memory_free_debug(&arena, a), "double free returns false");
    require_true(s_memory_free_debug(&arena, b), "s_free b");
    require_true(s_memory_free_debug(&arena, c2), "s_free c2");

    require_true(s_memory_arena_active_count(&arena) == 0, "active count zero");
    require_true(s_memory_arena_active_bytes(&arena) == 0, "active bytes zero");
    require_true(s_memory_arena_report_leaks(&arena, NULL), "no leaks");

    s_memory_arena_release(&arena);

    s_memory_arena arena_invalid = {0};
    require_true(s_memory_arena_init(&arena_invalid, 256), "arena_invalid init");
    u8* valid = (u8*)s_memory_malloc(&arena_invalid, 32);
    require_true(valid != NULL, "arena_invalid valid alloc");
    require_true(!s_memory_free_debug(&arena_invalid, valid + 1), "interior pointer free fails");
    require_true(s_memory_arena_active_count(&arena_invalid) == 1, "active count unchanged after interior free");
    require_true(s_memory_arena_active_bytes(&arena_invalid) == 32, "active bytes unchanged after interior free");
    require_true(!s_memory_realloc_debug(&arena_invalid, valid + 1, 64), "interior pointer realloc fails");
    require_true(!s_memory_free_debug(&arena_invalid, arena_invalid.seg_head->buffer + 100), "unused region free fails");
    require_true(s_memory_arena_active_count(&arena_invalid) == 1, "active count unchanged after invalid frees");
    require_true(s_memory_free_debug(&arena_invalid, valid), "valid free still works");
    require_true(s_memory_arena_report_leaks(&arena_invalid, NULL), "arena_invalid no leaks");
    s_memory_arena_release(&arena_invalid);

    s_memory_arena arena_overflow = {0};
    require_true(s_memory_arena_init(&arena_overflow, 256), "arena_overflow init");
    require_true(s_memory_malloc(&arena_overflow, SIZE_MAX) == NULL, "size_t overflow alloc fails");
    require_true(s_memory_arena_active_count(&arena_overflow) == 0, "overflow alloc keeps active count");
    require_true(s_memory_arena_active_bytes(&arena_overflow) == 0, "overflow alloc keeps active bytes");
    require_true(s_memory_arena_report_leaks(&arena_overflow, NULL), "arena_overflow no leaks");
    s_memory_arena_release(&arena_overflow);

    s_memory_arena arena_small = {0};
    require_true(s_memory_arena_init(&arena_small, 64), "arena_small init returns true");
    require_true(arena_small.seg_head != NULL, "arena_small init");
    sz initial_capacity = arena_small.seg_head->capacity;
    u8* big = (u8*)s_memory_malloc(&arena_small, 256);
    require_true(big != NULL, "arena grows on alloc");
    require_true(arena_small.seg_head->next != NULL, "arena grew new segment");
    require_true(s_memory_arena_active_count(&arena_small) == 1, "arena_small active count");
    s_memory_stats stats = s_memory_arena_get_stats(&arena_small);
    require_true(stats.capacity > initial_capacity, "arena_small capacity grew");
    require_true(stats.capacity >= stats.used, "arena_small capacity >= used");
    require_true(s_memory_free_debug(&arena_small, big), "arena_small free");
    require_true(s_memory_arena_report_leaks(&arena_small, NULL), "arena_small no leaks");
    s_memory_arena_release(&arena_small);
    printf("All s_memory tests passed.\n");
    return 0;
}
