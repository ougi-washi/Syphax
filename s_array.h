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
 * s_array_add(&my_array, value);
 * s_array_increment(&my_array);
 * s_array_handle(&my_array, dense_index);
 * s_array_get(&my_array, handle);
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
#include <assert.h>
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
    uc8* data;      		// byte buffer of T[size] 
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

static inline void s_array_reserve_dense(s_array_base* a, sz elem_size, sz need)
{
    if (need <= a->capacity) return;

    sz new_cap = a->capacity ? (a->capacity * 2) : (sz)16;
    if (new_cap < need) new_cap = need;

    s_assertf(elem_size != 0, "s_array :: elem_size=0\n");
    s_assertf(new_cap <= (SIZE_MAX / elem_size), "s_array :: data overflow\n");

    a->data = (unsigned char*)realloc(a->data, elem_size * new_cap);
    s_assertf(a->data != NULL, "s_array :: OOM data\n");

    a->dense_to_slot = (u32*)realloc(a->dense_to_slot, sizeof(u32) * new_cap);
    s_assertf(a->dense_to_slot != NULL, "s_array :: OOM dense_to_slot\n");

    a->capacity = new_cap;
}

static inline void s_array_reserve_slots(s_array_base* a, u32 need)
{
    if (need <= a->slot_cap) return;

    u32 new_cap = a->slot_cap ? (a->slot_cap * 2u) : 16u;
    if (new_cap < need) new_cap = need;

    a->slot_to_dense = (u32*)realloc(a->slot_to_dense, sizeof(u32) * new_cap);
    a->gen          = (u32*)realloc(a->gen,          sizeof(u32) * new_cap);
    s_assertf(a->slot_to_dense && a->gen, "s_array :: OOM slot tables\n");

    // init new entries 
    for (u32 i = a->slot_cap; i < new_cap; ++i) {
        a->slot_to_dense[i] = S_U32_INVALID;
        a->gen[i] = 1u; // generation never 0 (keeps handle 0 as null) 
    }

    a->slot_cap = new_cap;
}

static inline void s_array_reserve_free(s_array_base* a, u32 need)
{
    if (need <= a->free_cap) return;

    u32 new_cap = a->free_cap ? (a->free_cap * 2u) : 16u;
    if (new_cap < need) new_cap = need;

    a->free_slots = (u32*)realloc(a->free_slots, sizeof(u32) * new_cap);
    s_assertf(a->free_slots != NULL, "s_array :: OOM free_slots\n");

    a->free_cap = new_cap;
}

static inline u32 s_array_alloc_slot(s_array_base* a)
{
    if (a->free_count) {
        return a->free_slots[--a->free_count];
    }

    u32 slot = a->slot_count++;
    s_array_reserve_slots(a, a->slot_count);
    a->slot_to_dense[slot] = S_U32_INVALID;
    if (a->gen[slot] == 0u) a->gen[slot] = 1u;
    return slot;
}

static inline s_handle s_array_push_zero(s_array_base* a, sz elem_size)
{
    s_assertf(a->size <= (sz)UINT32_MAX, "s_array :: too many elems for u32\n");

    s_array_reserve_dense(a, elem_size, a->size + 1);

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
    s_handle h = s_array_push_zero(a, elem_size);
    u32 dense = a->slot_to_dense[s_handle_slot(h)];
    memcpy(a->data + (sz)dense * elem_size, value_ptr, elem_size);
    return h;
}

static inline void* s_array_get_ptr(const s_array_base* a, sz elem_size, s_handle h)
{
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

static inline s_handle s_array_handle_at(const s_array_base* a, u32 dense_index)
{
    if ((sz)dense_index >= a->size) return S_HANDLE_NULL;
    u32 slot = a->dense_to_slot[dense_index];
    return s_handle_make(slot, a->gen[slot]);
}

static inline void s_array_retire_slot(s_array_base* a, u32 slot)
{
    a->slot_to_dense[slot] = S_U32_INVALID;

    a->gen[slot]++;          // bump generation to invalidate old handles 
    if (a->gen[slot] == 0u) a->gen[slot] = 1u;

    s_array_reserve_free(a, a->free_count + 1);
    a->free_slots[a->free_count++] = slot;
}

// O(1) swap-remove: FAST, order NOT preserved 
static inline b8 s_array_remove_fast_base(s_array_base* a, sz elem_size, s_handle h)
{
    if (h == S_HANDLE_NULL) return 0;

    u32 slot = s_handle_slot(h);
    u32 gen  = s_handle_gen(h);

    if (slot >= a->slot_count) return 0;
    if (a->gen[slot] != gen) return 0;

    u32 dense = a->slot_to_dense[slot];
    if (dense == S_U32_INVALID) return 0;

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
    return 1;
}

// O(n) ordered remove: preserves packed order 
static inline b8 s_array_remove_ordered_base(s_array_base* a, sz elem_size, s_handle h)
{
    if (h == S_HANDLE_NULL) return 0;

    u32 slot = s_handle_slot(h);
    u32 gen  = s_handle_gen(h);

    if (slot >= a->slot_count) return 0;
    if (a->gen[slot] != gen) return 0;

    u32 dense = a->slot_to_dense[slot];
    if (dense == S_U32_INVALID) return 0;

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
    return 1;
}

static inline void s_array_clear_base(s_array_base* a)
{
    free(a->data);
    free(a->dense_to_slot);
    free(a->slot_to_dense);
    free(a->gen);
    free(a->free_slots);
    s_array_base_zero(a);
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

// Packed pointer to contiguous data (safe for iteration; don't keep across mutations) 
#define s_array_get_data(_arr) ((decltype((_arr)->tag))((void*)((_arr)->b.data)))

// Returns handle. _value must be an lvalue (so &(_value) is valid). 
#define s_array_add(_arr, _value_lvalue) \
    s_array_add_copy(&(_arr)->b, sizeof(*(_arr)->tag), (const void*)&(_value_lvalue))

// Returns handle to a zeroed element; fill it via s_array_get() 
#define s_array_increment(_arr) \
    s_array_push_zero(&(_arr)->b, sizeof(*(_arr)->tag))

// Dense index -> handle 
#define s_array_handle(_arr, _dense_index) \
    s_array_handle_at(&(_arr)->b, (u32)(_dense_index))

// Handle -> pointer (NULL if invalid/stale) 
#define s_array_get(_arr, _handle) \
    ((decltype((_arr)->tag))s_array_get_ptr(&(_arr)->b, sizeof(*(_arr)->tag), (_handle)))

// Swap-remove by handle (unordered but fast) 
#define s_array_remove(_arr, _handle) \
    s_array_remove_fast_base(&(_arr)->b, sizeof(*(_arr)->tag), (_handle))

// Remove (orered but slow O(n)) 
#define s_array_remove_ordered(_arr, _handle) \
    s_array_remove_ordered_base(&(_arr)->b, sizeof(*(_arr)->tag), (_handle))

#endif // S_ARRAY_H
