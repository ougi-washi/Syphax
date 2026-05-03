/*
 * Syphax - Ougi Washi
 *
 * Easy to use array library for C.
 * This approach is inspired by arena allocators but per array instead of being block-based.
 *
 * MIT License
 * Copyright (c) 2025-2026 Jed - Ougi Washi https://github.com/ougi-washi/Syphax */

/**
 * USAGE:
 *
 * s_array(my_struct, my_array);
 * s_array_init(&my_array);
 * s_array_get_size(&my_array);
 * s_array_get_capacity(&my_array);
 * s_array_get_data(&my_array);
 * s_array_reserve(&my_array, capacity);
 * s_array_add(&my_array, value);
 * s_array_append_many(&my_array, values, count);
 * s_array_increment(&my_array);
 * s_array_handle(&my_array, dense_index);
 * s_array_get(&my_array, handle);
 * s_array_copy(&dst_array, &src_array);
 * s_foreach(&my_array, it_ptr) { ... }
 * s_foreach_reverse(&my_array, it_ptr) { ... }
 * s_array_remove(&my_array, handle);
 * s_array_remove_ordered(&my_array, handle);
 * s_array_clear(&my_array);
 */

/* Example:
	typedef struct {
	    int x;
	    int y;
	} my_struct;

	s_array(my_struct, my_array);
	s_array_init(&my_array);

	my_struct value = { .x = 1, .y = 2 };
	s_handle h = s_array_add(&my_array, value);
	my_struct* p = s_array_get(&my_array, h);
	p->x = 3;

	for (sz i = 0; i < s_array_get_size(&my_array); ++i) {
	    s_handle hi = s_array_handle(&my_array, (u32)i);
	    my_struct* item = s_array_get(&my_array, hi);
	    printf("%d %d\n", item->x, item->y);
	}

	s_array_clear(&my_array);
*/


#ifndef S_ARRAY_H
#define S_ARRAY_H

#include "s_types.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>

typedef u64 s_handle;
#define S_HANDLE_NULL ((s_handle)0)
#define S_U32_INVALID ((u32)0xFFFFFFFFu)

static inline u32 s_handle_slot(s_handle h) { return (u32)(h & 0xFFFFFFFFull); }
static inline u32 s_handle_gen (s_handle h) { return (u32)(h >> 32); }
static inline s_handle s_handle_make(u32 slot, u32 gen) { return ((u64)gen << 32) | (u64)slot; }

typedef struct s_array_base {
    // packed elements
    uc8* data;              // byte buffer of T[size]
    u32* dense_to_slot;     // dense index -> slot

    // slot tables
    u32* slot_to_dense;     // slot -> dense index
    u32* gen;               // slot generation

    // free slot stack
    u32* free_slots;
    u32  free_count;
    u32  free_cap;

    sz size;                // alive count (packed)
    sz capacity;            // element capacity of packed arrays

    u32 slot_count;         // slots allocated so far
    u32 slot_cap;           // capacity of slot arrays
} s_array_base;

static inline void s_array_base_zero(s_array_base* a) { memset(a, 0, sizeof(*a)); }

static inline void* s_array_alloc_bytes(sz elem_size, sz count)
{
    if (elem_size == 0 || count > SIZE_MAX / elem_size) return NULL;
    return s_malloc(elem_size * count);
}

static inline b8 s_array_realloc_bytes(void** ptr, sz elem_size, sz count)
{
    if (ptr == NULL || elem_size == 0 || count > SIZE_MAX / elem_size) return false;
    void* next = s_realloc(*ptr, elem_size * count);
    if (next == NULL) return false;
    *ptr = next;
    return true;
}

static inline b8 s_array_ranges_overlap(const void* a, sz a_size, const void* b, sz b_size)
{
    const uintptr_t a_begin = (uintptr_t)a;
    const uintptr_t b_begin = (uintptr_t)b;
    if (a_size > UINTPTR_MAX - a_begin || b_size > UINTPTR_MAX - b_begin) return true;
    return b_begin < a_begin + a_size && b_begin + b_size > a_begin;
}

static inline b8 s_array_next_sz_cap(sz current, sz need, sz* out)
{
    if (out == NULL) return false;
    sz cap = current ? current : (sz)16;
    while (cap < need) {
        if (cap > SIZE_MAX / 2) {
            cap = need;
            break;
        }
        cap *= 2;
    }
    *out = cap;
    return true;
}

static inline b8 s_array_next_u32_cap(u32 current, u32 need, u32* out)
{
    if (out == NULL) return false;
    u32 cap = current ? current : 16u;
    while (cap < need) {
        if (cap > UINT32_MAX / 2u) {
            cap = need;
            break;
        }
        cap *= 2u;
    }
    *out = cap;
    return true;
}

static inline b8 s_array_reserve_base(s_array_base* a, sz elem_size, sz need)
{
    if (a == NULL || elem_size == 0) return false;
    if (need > (sz)UINT32_MAX) return false;

    u32 slot_need = a->slot_count;
    if (need > a->size) {
        sz add_need = need - a->size;
        if (add_need > (sz)a->free_count) {
            sz extra_slots = add_need - (sz)a->free_count;
            if (extra_slots > (sz)(UINT32_MAX - a->slot_count)) return false;
            slot_need = a->slot_count + (u32)extra_slots;
        }
    }

    if (need > a->capacity) {
        sz new_dense_cap = 0;
        uc8* data = NULL;
        u32* dense_to_slot = NULL;
        if (!s_array_next_sz_cap(a->capacity, need, &new_dense_cap)) return false;
        if (new_dense_cap > SIZE_MAX / elem_size) return false;

        data = (uc8*)s_array_alloc_bytes(elem_size, new_dense_cap);
        dense_to_slot = (u32*)s_array_alloc_bytes(sizeof(u32), new_dense_cap);
        if (data == NULL || dense_to_slot == NULL) {
            s_free(data);
            s_free(dense_to_slot);
            return false;
        }
        if (a->size > 0 && a->data != NULL && a->dense_to_slot != NULL) {
            memcpy(data, a->data, elem_size * a->size);
            memcpy(dense_to_slot, a->dense_to_slot, sizeof(u32) * a->size);
        }
        s_free(a->data);
        s_free(a->dense_to_slot);
        a->data = data;
        a->dense_to_slot = dense_to_slot;

        a->capacity = new_dense_cap;
    }

    if (slot_need > a->slot_cap) {
        u32 new_slot_cap = 0;
        u32* slot_to_dense = NULL;
        u32* gen = NULL;
        if (!s_array_next_u32_cap(a->slot_cap, slot_need, &new_slot_cap)) return false;
        slot_to_dense = (u32*)s_array_alloc_bytes(sizeof(u32), (sz)new_slot_cap);
        gen = (u32*)s_array_alloc_bytes(sizeof(u32), (sz)new_slot_cap);
        if (slot_to_dense == NULL || gen == NULL) {
            s_free(slot_to_dense);
            s_free(gen);
            return false;
        }
        if (a->slot_cap > 0 && a->slot_to_dense != NULL && a->gen != NULL) {
            memcpy(slot_to_dense, a->slot_to_dense, sizeof(u32) * (sz)a->slot_cap);
            memcpy(gen, a->gen, sizeof(u32) * (sz)a->slot_cap);
        }

        // init new entries
        for (u32 i = a->slot_cap; i < new_slot_cap; ++i) {
            slot_to_dense[i] = S_U32_INVALID;
            gen[i] = 1u; // generation never 0 (keeps handle 0 as null)
        }

        s_free(a->slot_to_dense);
        s_free(a->gen);
        a->slot_to_dense = slot_to_dense;
        a->gen = gen;
        a->slot_cap = new_slot_cap;
    }

    return true;
}

static inline u32 s_array_alloc_slot(s_array_base* a)
{
    if (a->free_count) {
        return a->free_slots[--a->free_count];
    }

    s_assertf(a->slot_count < UINT32_MAX, "s_array :: too many slots for u32\n");
    u32 slot = a->slot_count++;
    s_assertf(a->slot_count <= a->slot_cap, "s_array :: slot capacity not reserved\n");
    a->slot_to_dense[slot] = S_U32_INVALID;
    if (a->gen[slot] == 0u) a->gen[slot] = 1u;
    return slot;
}

static inline s_handle s_array_push_zero(s_array_base* a, sz elem_size)
{
    if (a == NULL || elem_size == 0) return S_HANDLE_NULL;
    if (a->size >= (sz)UINT32_MAX) return S_HANDLE_NULL;
    if (!a->free_count && a->slot_count >= UINT32_MAX) return S_HANDLE_NULL;
    if (a->size > SIZE_MAX - 1) return S_HANDLE_NULL;

    if (!s_array_reserve_base(a, elem_size, a->size + 1)) return S_HANDLE_NULL;

    u32 slot  = s_array_alloc_slot(a);
    u32 dense = (u32)a->size++;

    a->slot_to_dense[slot] = dense;
    a->dense_to_slot[dense] = slot;

    void* p = (void*)(a->data + (sz)dense * elem_size);
    memset(p, 0, elem_size);

    return s_handle_make(slot, a->gen[slot]);
}

static inline s_handle s_array_add_copy(s_array_base* a, sz elem_size, const void* value_ptr)
{
    if (value_ptr == NULL) return S_HANDLE_NULL;
    s_handle h = s_array_push_zero(a, elem_size);
    if (h == S_HANDLE_NULL) return S_HANDLE_NULL;
    u32 dense = a->slot_to_dense[s_handle_slot(h)];
    memcpy(a->data + (sz)dense * elem_size, value_ptr, elem_size);
    return h;
}

static inline s_handle s_array_append_many_copy(s_array_base* a, sz elem_size, const void* values_ptr, sz count)
{
    if (count == 0) return S_HANDLE_NULL;

    if (a == NULL || elem_size == 0 || values_ptr == NULL) return S_HANDLE_NULL;
    if (a->size > (SIZE_MAX - count)) return S_HANDLE_NULL;

    sz old_size = a->size;
    sz new_size = old_size + count;
    if (new_size > (sz)UINT32_MAX) return S_HANDLE_NULL;

    const uc8* values = (const uc8*)values_ptr;
    uc8* temp_values = NULL;
    if (a->data != NULL && a->size > 0) {
        if (old_size > SIZE_MAX / elem_size) return S_HANDLE_NULL;
        if (count > SIZE_MAX / elem_size) return S_HANDLE_NULL;
        const sz data_bytes = old_size * elem_size;
        const sz values_bytes = count * elem_size;
        if (s_array_ranges_overlap(a->data, data_bytes, values, values_bytes)) {
            temp_values = (uc8*)s_malloc(values_bytes);
            if (temp_values == NULL) return S_HANDLE_NULL;
            memcpy(temp_values, values_ptr, values_bytes);
            values = temp_values;
        }
    }

    if (!s_array_reserve_base(a, elem_size, new_size)) {
        s_free(temp_values);
        return S_HANDLE_NULL;
    }

    s_handle first = S_HANDLE_NULL;
    if (a->free_count == 0) {
        if (count > (sz)(UINT32_MAX - a->slot_count)) {
            s_free(temp_values);
            return S_HANDLE_NULL;
        }

        u32 first_slot = a->slot_count;
        a->slot_count += (u32)count;
        first = s_handle_make(first_slot, a->gen[first_slot]);

        for (sz i = 0; i < count; ++i) {
            u32 slot = first_slot + (u32)i;
            u32 dense = (u32)(old_size + i);

            a->slot_to_dense[slot] = dense;
            a->dense_to_slot[dense] = slot;
        }
    } else {
        for (sz i = 0; i < count; ++i) {
            u32 slot = s_array_alloc_slot(a);
            u32 dense = (u32)(old_size + i);

            a->slot_to_dense[slot] = dense;
            a->dense_to_slot[dense] = slot;

            if (i == 0) {
                first = s_handle_make(slot, a->gen[slot]);
            }
        }
    }

    memcpy(a->data + old_size * elem_size, values, count * elem_size);
    s_free(temp_values);
    a->size = new_size;
    return first;
}

static inline void* s_array_get_ptr(const s_array_base* a, sz elem_size, s_handle h)
{
    if (a == NULL || elem_size == 0) return NULL;
    if (h == S_HANDLE_NULL) return NULL;

    u32 slot = s_handle_slot(h);
    u32 gen  = s_handle_gen(h);

    if (slot >= a->slot_count) return NULL;
    if (a->gen[slot] != gen) return NULL;

    u32 dense = a->slot_to_dense[slot];
    if (dense == S_U32_INVALID) return NULL;
    if ((sz)dense >= a->size) return NULL;

    return (void*)(a->data + (sz)dense * elem_size);
}

static inline s_handle s_array_handle_at(const s_array_base* a, sz dense_index)
{
    if (a == NULL) return S_HANDLE_NULL;
    if (dense_index > (sz)UINT32_MAX || dense_index >= a->size) return S_HANDLE_NULL;
    u32 slot = a->dense_to_slot[dense_index];
    return s_handle_make(slot, a->gen[slot]);
}

static inline b8 s_array_ensure_free_slot(s_array_base* a)
{
    if (a == NULL) return false;
    if (a->free_count < a->free_cap) return true;
    if (a->free_count == UINT32_MAX) return false;
    u32 new_free_cap = 0;
    if (!s_array_next_u32_cap(a->free_cap, a->free_count + 1, &new_free_cap)) return false;
    if (!s_array_realloc_bytes((void**)&a->free_slots, sizeof(u32), (sz)new_free_cap)) return false;
    a->free_cap = new_free_cap;
    return true;
}

static inline void s_array_retire_slot(s_array_base* a, u32 slot)
{
    a->slot_to_dense[slot] = S_U32_INVALID;

    a->gen[slot]++;          // bump generation to invalidate old handles
    if (a->gen[slot] == 0u) a->gen[slot] = 1u;
    a->free_slots[a->free_count++] = slot;
}

// O(1) swap-remove: FAST, order NOT preserved
static inline b8 s_array_remove_fast_base(s_array_base* a, sz elem_size, s_handle h)
{
    if (a == NULL || elem_size == 0 || a->size == 0) return false;
    if (h == S_HANDLE_NULL) return false;

    u32 slot = s_handle_slot(h);
    u32 gen  = s_handle_gen(h);

    if (slot >= a->slot_count) return false;
    if (a->gen[slot] != gen) return false;

    u32 dense = a->slot_to_dense[slot];
    if (dense == S_U32_INVALID) return false;
    if (!s_array_ensure_free_slot(a)) return false;

    u32 last = (u32)(a->size - 1);
    if (dense != last) {
        // move last element into dense
        memcpy(a->data + (sz)dense * elem_size,
               a->data + (sz)last  * elem_size,
               elem_size);

        u32 moved_slot = a->dense_to_slot[last];
        a->dense_to_slot[dense] = moved_slot;
        a->slot_to_dense[moved_slot] = dense;
    }

    a->size--;
    s_array_retire_slot(a, slot);
    return true;
}

// O(n) ordered remove: preserves packed order
static inline b8 s_array_remove_ordered_base(s_array_base* a, sz elem_size, s_handle h)
{
    if (a == NULL || elem_size == 0 || a->size == 0) return false;
    if (h == S_HANDLE_NULL) return false;

    u32 slot = s_handle_slot(h);
    u32 gen  = s_handle_gen(h);

    if (slot >= a->slot_count) return false;
    if (a->gen[slot] != gen) return false;

    u32 dense = a->slot_to_dense[slot];
    if (dense == S_U32_INVALID) return false;
    if (!s_array_ensure_free_slot(a)) return false;

    u32 last = (u32)(a->size - 1);
    if (dense != last) {
        // shift data left
        memmove(a->data + (sz)dense * elem_size,
                a->data + (sz)(dense + 1) * elem_size,
                (sz)(last - dense) * elem_size);

        // shift dense_to_slot left
        memmove(&a->dense_to_slot[dense],
                &a->dense_to_slot[dense + 1],
                sizeof(u32) * (sz)(last - dense));

        // fix slot_to_dense for all shifted items
        for (u32 i = dense; i < last; ++i) {
            u32 s = a->dense_to_slot[i];
            a->slot_to_dense[s] = i;
        }
    }

    a->size--;
    s_array_retire_slot(a, slot);
    return true;
}

static inline void s_array_clear_base(s_array_base* a)
{
    if (a == NULL) return;
    s_free(a->data);
    s_free(a->dense_to_slot);
    s_free(a->slot_to_dense);
    s_free(a->gen);
    s_free(a->free_slots);
    s_array_base_zero(a);
}

static inline b8 s_array_copy_base(s_array_base* dst, const s_array_base* src, sz elem_size)
{
    if (dst == NULL || elem_size == 0) return false;
    if (dst == src) return true;
    if (!src) {
        s_array_clear_base(dst);
        return true;
    }

    s_array_base next;
    s_array_base_zero(&next);
    next.size = src->size;
    next.capacity = src->capacity;
    next.slot_count = src->slot_count;
    next.slot_cap = src->slot_cap;
    next.free_count = src->free_count;
    next.free_cap = src->free_cap;

    if (next.capacity) {
        next.data = (uc8*)s_array_alloc_bytes(elem_size, next.capacity);
        next.dense_to_slot = (u32*)s_array_alloc_bytes(sizeof(u32), next.capacity);
        if (next.data == NULL || next.dense_to_slot == NULL) goto fail;

        if (next.size) {
            memcpy(next.data, src->data, elem_size * next.size);
            memcpy(next.dense_to_slot, src->dense_to_slot, sizeof(u32) * next.size);
        }
    }

    if (next.slot_cap) {
        next.slot_to_dense = (u32*)s_array_alloc_bytes(sizeof(u32), next.slot_cap);
        next.gen = (u32*)s_array_alloc_bytes(sizeof(u32), next.slot_cap);
        if (next.slot_to_dense == NULL || next.gen == NULL) goto fail;
        memcpy(next.slot_to_dense, src->slot_to_dense, sizeof(u32) * next.slot_cap);
        memcpy(next.gen, src->gen, sizeof(u32) * next.slot_cap);
    }

    if (next.free_cap) {
        next.free_slots = (u32*)s_array_alloc_bytes(sizeof(u32), next.free_cap);
        if (next.free_slots == NULL) goto fail;
        if (next.free_count) memcpy(next.free_slots, src->free_slots, sizeof(u32) * next.free_count);
    }

    s_array_clear_base(dst);
    *dst = next;
    return true;

fail:
    s_array_clear_base(&next);
    return false;
}

// USAGE:

#define s_array(_type, _name) \
    struct { s_array_base b; _type* tag; } _name

#define s_array_init(_arr) \
    do { s_array_base_zero(&(_arr)->b); (_arr)->tag = NULL; } while (0)

#define s_array_clear(_arr) \
    do { s_array_clear_base(&(_arr)->b); (_arr)->tag = NULL; } while (0)

#define s_array_get_size(_arr)     ((_arr)->b.size)
#define s_array_get_capacity(_arr) ((_arr)->b.capacity)

// Packed pointer to contiguous data (safe for iteration; don't keep across mutations).
// Any mutation (add/remove/reserve/clear/copy) may invalidate previously fetched pointers.
#define s_array_get_data(_arr) ((_arr)->tag = (void*)((_arr)->b.data), (_arr)->tag)

// Reserve packed capacity (and matching slot capacity) to at least _need elements.
// Preserves packed order and keeps existing valid handles valid.
#define s_array_reserve(_arr, _need) \
    s_array_reserve_base(&(_arr)->b, sizeof(*(_arr)->tag), (sz)(_need))

// Returns handle. _value must be an lvalue (so &(_value) is valid).
#define s_array_add(_arr, _value_lvalue) \
    s_array_add_copy(&(_arr)->b, sizeof(*(_arr)->tag), (const void*)&(_value_lvalue))

// Appends _count values from _values. Returns first appended handle, or S_HANDLE_NULL for 0.
#define s_array_append_many(_arr, _values, _count) \
    s_array_append_many_copy(&(_arr)->b, sizeof(*(_arr)->tag), (const void*)(_values), (sz)(_count))

// Returns handle to a zeroed element; fill it via s_array_get()
#define s_array_increment(_arr) \
    s_array_push_zero(&(_arr)->b, sizeof(*(_arr)->tag))

// Dense index -> handle
#define s_array_handle(_arr, _dense_index) \
    s_array_handle_at(&(_arr)->b, (sz)(_dense_index))

// Handle -> pointer (NULL if invalid/stale)
#define s_array_get(_arr, _handle) \
    ((_arr)->tag = s_array_get_ptr(&(_arr)->b, sizeof(*(_arr)->tag), (_handle)), (_arr)->tag)

#define s_array_copy(_dst, _src) \
    (s_array_copy_base(&(_dst)->b, &(_src)->b, sizeof(*(_src)->tag)) ? ((_dst)->tag = NULL, true) : false)

#define s_foreach(_arr, _it) \
    for (sz _it##_index = 0, _it##_count = s_array_get_size(_arr); _it##_index < _it##_count; ++_it##_index) \
        for (s_handle _it##_handle = s_array_handle((_arr), (u32)_it##_index); _it##_handle != S_HANDLE_NULL; _it##_handle = S_HANDLE_NULL) \
            for ((_it) = s_array_get((_arr), _it##_handle); (_it) != NULL; (_it) = NULL)

#define s_foreach_reverse(_arr, _it) \
    for (sz _it##_count = s_array_get_size(_arr), _it##_index = _it##_count; _it##_index-- > 0; ) \
        for (s_handle _it##_handle = s_array_handle((_arr), (u32)_it##_index); _it##_handle != S_HANDLE_NULL; _it##_handle = S_HANDLE_NULL) \
            for ((_it) = s_array_get((_arr), _it##_handle); (_it) != NULL; (_it) = NULL)

// Swap-remove by handle (unordered but fast)
#define s_array_remove(_arr, _handle) \
    s_array_remove_fast_base(&(_arr)->b, sizeof(*(_arr)->tag), (_handle))

// Remove (orered but slow O(n))
#define s_array_remove_ordered(_arr, _handle) \
    s_array_remove_ordered_base(&(_arr)->b, sizeof(*(_arr)->tag), (_handle))

#endif // S_ARRAY_H
