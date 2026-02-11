/**
 * Syphax - Ougi Washi
 *
 * Simple arena-style allocator with debug tracking.
 * Tracks allocation file/line and avoids crashing on double free.
 *
 * MIT License
 * Copyright (c) 2025-2026 Jed - Ougi Washi https://github.com/ougi-washi/Syphax
 */

#ifndef S_MEMORY_H
#define S_MEMORY_H

#include "s_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>

#ifndef s_memory_ALIGNMENT
#define s_memory_ALIGNMENT ((sz)sizeof(void*))
#endif

#ifndef s_memory_MIN_SEGMENT
#define s_memory_MIN_SEGMENT ((sz)1024)
#endif

typedef struct s_memory_block {
    const char* file;
    i32 line;
    sz size;
    b8 freed;
    struct s_memory_block* next;
} s_memory_block;

typedef struct s_memory_segment {
    u8* buffer;
    sz capacity;
    sz offset;
    b8 owns_buffer;
    struct s_memory_segment* next;
} s_memory_segment;

typedef struct {
    s_memory_segment* seg_head;
    s_memory_segment* seg_tail;
    s_memory_block* block_head;
    s_memory_block* block_tail;
    sz alloc_count;
    sz free_count;
    sz bytes_allocated;
    sz bytes_freed;
} s_memory_arena;

typedef struct {
    sz alloc_count;
    sz free_count;
    sz active_count;
    sz bytes_allocated;
    sz bytes_freed;
    sz active_bytes;
    sz capacity;
    sz used;
} s_memory_stats;

static inline b8 s_memory_align_up_checked(sz _value, sz _align, sz* _out) {
    if (_out == NULL) return false;
    if (_align == 0) {
        *_out = _value;
        return true;
    }

    sz rem = _value % _align;
    sz add = (rem == 0) ? 0 : (_align - rem);
    if (_value > (SIZE_MAX - add)) return false;
    *_out = _value + add;
    return true;
}

static inline sz s_memory_align_up(sz _value, sz _align) {
    sz out = _value;
    if (!s_memory_align_up_checked(_value, _align, &out)) return SIZE_MAX;
    return out;
}

static inline sz s_memory_block_size(void) {
    return s_memory_align_up((sz)sizeof(s_memory_block), s_memory_ALIGNMENT);
}

static inline s_memory_segment* s_memory_segment_create(void* _buffer, sz _capacity, b8 _owns_buffer) {
    if (_capacity == 0) return NULL;
    s_memory_segment* seg = (s_memory_segment*)malloc(sizeof(s_memory_segment));
    if (seg == NULL) return NULL;
    seg->buffer = (u8*)_buffer;
    seg->capacity = _capacity;
    seg->offset = 0;
    seg->owns_buffer = _owns_buffer;
    seg->next = NULL;
    if (seg->buffer == NULL) {
        if (!_owns_buffer) {
            free(seg);
            return NULL;
        }
        seg->buffer = (u8*)malloc(_capacity);
        if (seg->buffer == NULL) {
            free(seg);
            return NULL;
        }
    }
    return seg;
}

static inline void s_memory_segment_destroy(s_memory_segment* _seg) {
    if (_seg == NULL) return;
    if (_seg->owns_buffer && _seg->buffer != NULL) {
        free(_seg->buffer);
    }
    free(_seg);
}

static inline void s_memory_arena_init(s_memory_arena* _arena, sz _capacity) {
    s_assertf(_arena != NULL, "s_memory_arena_init :: Arena is null\n");
    _arena->seg_head = NULL;
    _arena->seg_tail = NULL;
    _arena->block_head = NULL;
    _arena->block_tail = NULL;
    _arena->alloc_count = 0;
    _arena->free_count = 0;
    _arena->bytes_allocated = 0;
    _arena->bytes_freed = 0;
    if (_capacity == 0) return;
    s_memory_segment* seg = s_memory_segment_create(NULL, _capacity, true);
    if (seg == NULL) return;
    _arena->seg_head = seg;
    _arena->seg_tail = seg;
}

static inline void s_memory_arena_init_with_buffer(s_memory_arena* _arena, void* _buffer, sz _capacity) {
    s_assertf(_arena != NULL, "s_memory_arena_init_with_buffer :: Arena is null\n");
    _arena->seg_head = NULL;
    _arena->seg_tail = NULL;
    _arena->block_head = NULL;
    _arena->block_tail = NULL;
    _arena->alloc_count = 0;
    _arena->free_count = 0;
    _arena->bytes_allocated = 0;
    _arena->bytes_freed = 0;
    if (_capacity == 0 || _buffer == NULL) return;
    s_memory_segment* seg = s_memory_segment_create(_buffer, _capacity, false);
    if (seg == NULL) return;
    _arena->seg_head = seg;
    _arena->seg_tail = seg;
}

static inline void s_memory_arena_reset(s_memory_arena* _arena) {
    if (_arena == NULL) return;
    if (_arena->seg_head != NULL) {
        s_memory_segment* keep = _arena->seg_head;
        s_memory_segment* seg = keep->next;
        while (seg != NULL) {
            s_memory_segment* next = seg->next;
            s_memory_segment_destroy(seg);
            seg = next;
        }
        keep->offset = 0;
        keep->next = NULL;
        _arena->seg_tail = keep;
    }
    _arena->block_head = NULL;
    _arena->block_tail = NULL;
    _arena->alloc_count = 0;
    _arena->free_count = 0;
    _arena->bytes_allocated = 0;
    _arena->bytes_freed = 0;
}

static inline void s_memory_arena_release(s_memory_arena* _arena) {
    if (_arena == NULL) return;
    s_memory_segment* seg = _arena->seg_head;
    while (seg != NULL) {
        s_memory_segment* next = seg->next;
        s_memory_segment_destroy(seg);
        seg = next;
    }
    _arena->seg_head = NULL;
    _arena->seg_tail = NULL;
    _arena->block_head = NULL;
    _arena->block_tail = NULL;
    _arena->alloc_count = 0;
    _arena->free_count = 0;
    _arena->bytes_allocated = 0;
    _arena->bytes_freed = 0;
}

static inline b8 s_memory_arena_contains(const s_memory_arena* _arena, const void* _ptr) {
    if (_arena == NULL || _ptr == NULL) return false;
    const s_memory_segment* seg = _arena->seg_head;
    const u8* ptr = (const u8*)_ptr;
    while (seg != NULL) {
        const u8* start = seg->buffer;
        sz used = seg->offset;
        if (used > seg->capacity) used = seg->capacity;
        const u8* end = seg->buffer + used;
        if (ptr >= start && ptr < end) return true;
        seg = seg->next;
    }
    return false;
}

static inline s_memory_segment* s_memory_arena_segment_from_ptr(const s_memory_arena* _arena, const void* _ptr) {
    if (_arena == NULL || _ptr == NULL) return NULL;
    s_memory_segment* seg = _arena->seg_head;
    const u8* ptr = (const u8*)_ptr;
    while (seg != NULL) {
        const u8* start = seg->buffer;
        sz used = seg->offset;
        if (used > seg->capacity) used = seg->capacity;
        const u8* end = seg->buffer + used;
        if (ptr >= start && ptr < end) return seg;
        seg = seg->next;
    }
    return NULL;
}

static inline s_memory_block* s_memory_arena_block_from_ptr(const s_memory_arena* _arena, const void* _ptr) {
    if (_arena == NULL || _ptr == NULL) return NULL;
    const sz header_size = s_memory_block_size();
    const u8* target = (const u8*)_ptr;
    s_memory_block* block = _arena->block_head;
    while (block != NULL) {
        const u8* payload = (const u8*)block + header_size;
        if (payload == target) return block;
        block = block->next;
    }
    return NULL;
}

static inline b8 s_memory_arena_grow(s_memory_arena* _arena, sz _min_capacity) {
    if (_arena == NULL) return false;
    sz new_cap = _min_capacity;
    if (_arena->seg_tail != NULL) {
        sz base = _arena->seg_tail->capacity;
        if (base == 0) base = s_memory_MIN_SEGMENT;
        new_cap = base;
        while (new_cap < _min_capacity) {
            if (new_cap > (SIZE_MAX / 2)) {
                new_cap = _min_capacity;
                break;
            }
            new_cap *= 2;
        }
    } else if (new_cap < s_memory_MIN_SEGMENT) {
        new_cap = s_memory_MIN_SEGMENT;
    }

    s_memory_segment* seg = s_memory_segment_create(NULL, new_cap, true);
    if (seg == NULL) return false;
    if (_arena->seg_tail != NULL) {
        _arena->seg_tail->next = seg;
        _arena->seg_tail = seg;
    } else {
        _arena->seg_head = seg;
        _arena->seg_tail = seg;
    }
    return true;
}

static inline void* s_memory_alloc(s_memory_arena* _arena, sz _size, const char* _file, i32 _line) {
    if (_arena == NULL || _size == 0) return NULL;
    const sz header_size = s_memory_block_size();
    if (header_size > (SIZE_MAX - _size)) return NULL;
    sz required = header_size + _size;
    sz total = 0;
    if (!s_memory_align_up_checked(required, s_memory_ALIGNMENT, &total)) return NULL;
    if (total < required) return NULL;

    s_memory_segment* seg = _arena->seg_tail;
    if (seg == NULL) {
        if (!s_memory_arena_grow(_arena, total)) return NULL;
        seg = _arena->seg_tail;
    }

    sz aligned_offset = 0;
    if (!s_memory_align_up_checked(seg->offset, s_memory_ALIGNMENT, &aligned_offset)) return NULL;
    if (total > seg->capacity || aligned_offset > (seg->capacity - total)) {
        if (!s_memory_arena_grow(_arena, total)) return NULL;
        seg = _arena->seg_tail;
        if (!s_memory_align_up_checked(seg->offset, s_memory_ALIGNMENT, &aligned_offset)) return NULL;
        if (total > seg->capacity || aligned_offset > (seg->capacity - total)) return NULL;
    }

    s_memory_block* block = (s_memory_block*)(seg->buffer + aligned_offset);
    block->file = _file;
    block->line = _line;
    block->size = _size;
    block->freed = false;
    block->next = NULL;

    if (_arena->block_tail != NULL) {
        _arena->block_tail->next = block;
    } else {
        _arena->block_head = block;
    }
    _arena->block_tail = block;

    seg->offset = aligned_offset + total;
    _arena->alloc_count += 1;
    _arena->bytes_allocated += _size;

    return (void*)((u8*)block + header_size);
}

static inline b8 s_memory_free(s_memory_arena* _arena, void* _ptr, const char* _file, i32 _line) {
    (void)_file;
    (void)_line;
    if (_ptr == NULL) return true;
    if (_arena == NULL) return false;

    s_memory_block* block = s_memory_arena_block_from_ptr(_arena, _ptr);
    if (block == NULL) return false;
    if (block->freed) return false;

    block->freed = true;
    _arena->free_count += 1;
    _arena->bytes_freed += block->size;
    return true;
}

static inline void* s_memory_calloc(s_memory_arena* _arena, sz _count, sz _size, const char* _file, i32 _line) {
    if (_count == 0 || _size == 0) return NULL;
    if (_count > (SIZE_MAX / _size)) return NULL;
    sz total = _count * _size;
    void* ptr = s_memory_alloc(_arena, total, _file, _line);
    if (ptr == NULL) return NULL;
    memset(ptr, 0, total);
    return ptr;
}

static inline void* s_memory_realloc(s_memory_arena* _arena, void* _ptr, sz _size, const char* _file, i32 _line) {
    if (_ptr == NULL) return s_memory_alloc(_arena, _size, _file, _line);
    if (_size == 0) {
        s_memory_free(_arena, _ptr, _file, _line);
        return NULL;
    }
    if (_arena == NULL) return NULL;
    s_memory_block* block = s_memory_arena_block_from_ptr(_arena, _ptr);
    if (block == NULL) return NULL;
    if (block->freed) return NULL;

    void* next = s_memory_alloc(_arena, _size, _file, _line);
    if (next == NULL) return NULL;
    sz copy_size = block->size < _size ? block->size : _size;
    memcpy(next, _ptr, copy_size);
    s_memory_free(_arena, _ptr, _file, _line);
    return next;
}

static inline s_memory_stats s_memory_arena_get_stats(const s_memory_arena* _arena) {
    s_memory_stats stats = {0};
    if (_arena == NULL) return stats;
    stats.alloc_count = _arena->alloc_count;
    stats.free_count = _arena->free_count;
    stats.active_count = (_arena->alloc_count >= _arena->free_count) ? (_arena->alloc_count - _arena->free_count) : 0;
    stats.bytes_allocated = _arena->bytes_allocated;
    stats.bytes_freed = _arena->bytes_freed;
    stats.active_bytes = (_arena->bytes_allocated >= _arena->bytes_freed) ? (_arena->bytes_allocated - _arena->bytes_freed) : 0;

    sz capacity = 0;
    sz used = 0;
    const s_memory_segment* seg = _arena->seg_head;
    while (seg != NULL) {
        capacity += seg->capacity;
        used += seg->offset;
        seg = seg->next;
    }
    stats.capacity = capacity;
    stats.used = used;
    return stats;
}

static inline sz s_memory_arena_active_count(const s_memory_arena* _arena) {
    if (_arena == NULL) return 0;
    return (_arena->alloc_count >= _arena->free_count) ? (_arena->alloc_count - _arena->free_count) : 0;
}

static inline sz s_memory_arena_active_bytes(const s_memory_arena* _arena) {
    if (_arena == NULL) return 0;
    return (_arena->bytes_allocated >= _arena->bytes_freed) ? (_arena->bytes_allocated - _arena->bytes_freed) : 0;
}

static inline sz s_memory_arena_bytes_for_file(const s_memory_arena* _arena, const char* _file) {
    if (_arena == NULL || _file == NULL) return 0;
    sz total = 0;
    const s_memory_block* block = _arena->block_head;
    while (block != NULL) {
        if (!block->freed && block->file != NULL && strcmp(block->file, _file) == 0) {
            total += block->size;
        }
        block = block->next;
    }
    return total;
}

static inline sz s_memory_arena_bytes_for_line(const s_memory_arena* _arena, const char* _file, i32 _line) {
    if (_arena == NULL || _file == NULL) return 0;
    sz total = 0;
    const s_memory_block* block = _arena->block_head;
    while (block != NULL) {
        if (!block->freed && block->file != NULL && block->line == _line && strcmp(block->file, _file) == 0) {
            total += block->size;
        }
        block = block->next;
    }
    return total;
}

static inline void s_memory_arena_dump(const s_memory_arena* _arena, FILE* _out, b8 _include_freed) {
    if (_out == NULL) _out = stderr;
    if (_arena == NULL) {
        fprintf(_out, "s_memory_arena_dump: null arena\n");
        return;
    }
    s_memory_stats stats = s_memory_arena_get_stats(_arena);
    fprintf(_out, "s_memory_arena_dump: capacity=%zu used=%zu allocs=%zu frees=%zu\n",
        stats.capacity, stats.used, stats.alloc_count, stats.free_count);
    const s_memory_block* block = _arena->block_head;
    const sz header_size = s_memory_block_size();
    while (block != NULL) {
        if (_include_freed || !block->freed) {
            const void* payload = (const void*)((const u8*)block + header_size);
            fprintf(_out, "  %p size=%zu freed=%d at %s:%d\n",
                payload, block->size, block->freed ? 1 : 0,
                block->file ? block->file : "(unknown)", block->line);
        }
        block = block->next;
    }
}

static inline b8 s_memory_arena_report_leaks(const s_memory_arena* _arena, FILE* _out) {
    if (_out == NULL) _out = stderr;
    if (_arena == NULL) {
        fprintf(_out, "s_memory_arena_report_leaks: null arena\n");
        return false;
    }
    sz leak_count = 0;
    sz leak_bytes = 0;
    const s_memory_block* block = _arena->block_head;
    const sz header_size = s_memory_block_size();
    while (block != NULL) {
        if (!block->freed) {
            leak_count += 1;
            leak_bytes += block->size;
            const void* payload = (const void*)((const u8*)block + header_size);
            fprintf(_out, "LEAK %p size=%zu at %s:%d\n",
                payload, block->size,
                block->file ? block->file : "(unknown)", block->line);
        }
        block = block->next;
    }
    if (leak_count == 0) {
        fprintf(_out, "No leaks detected.\n");
        return true;
    }
    fprintf(_out, "Leaks: %zu allocations, %zu bytes.\n", leak_count, leak_bytes);
    return false;
}

#define s_malloc(_arena, _size) s_memory_alloc((_arena), (_size), __FILE__, __LINE__)
#define s_calloc(_arena, _count, _size) s_memory_calloc((_arena), (_count), (_size), __FILE__, __LINE__)
#define s_realloc(_arena, _ptr, _size) s_memory_realloc((_arena), (_ptr), (_size), __FILE__, __LINE__)
#define s_free(_arena, _ptr) s_memory_free((_arena), (_ptr), __FILE__, __LINE__)

#endif // S_MEMORY_H
