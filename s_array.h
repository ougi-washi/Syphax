/**
 * Syphax - Ougi Washi
 *
 * Easy to use array library for C.
 * This approach is inspired by arena allocators but per array instead of being block-based.
 * This is not a dynamic array but a static array with a fixed size.
 *
 * MIT License
 * Copyright (c) 2025-2026 Jed - Ougi Washi https://github.com/ougi-washi/Syphax
 */

#ifndef S_ARRAY_H
#define S_ARRAY_H

#include "s_types.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

static inline void s_array_require(b8 _cond, const char* _msg) {
    s_assertf(_cond, "%s", _msg);
}

#define s_array(_type, _array) \
    struct { \
        _type* data; \
        sz size; \
        sz capacity; \
    } _array

#define s_array_get(_array, _index) \
    (&((_array)->data[(_index)]))
    
#define s_array_get_data(_array) \
    ((_array)->data)

#define s_array_get_size(_array) \
    ((_array)->size)

#define s_array_get_capacity(_array) \
    ((_array)->capacity)

#define s_foreach(_array, _it) \
    s_assertf((_array) != NULL, "s_foreach :: Array is null\n"); \
    for (sz _it = 0; _it < (_array)->size; _it++)

#define s_foreach_reverse(_array, _it) \
    s_assertf((_array) != NULL, "s_foreach_reverse :: Array is null\n"); \
    for (sz _it = (_array)->size; _it-- > 0;)

#define s_array_memset(_array, _value) \
    do { \
        s_assertf((_array) != NULL, "s_array_memset :: Array is null\n"); \
        if ((_array)->data != NULL && (_array)->size > 0) { \
            memset((_array)->data, (_value), sizeof(*((_array)->data)) * (_array)->size); \
        } \
    } while (0)

#define s_array_init(_array, _size) \
    do { \
        s_assertf((_array) != NULL, "s_array_init :: Array is null\n"); \
        sz _capacity = (sz)(_size); \
        if (_capacity > 0) { \
            s_assertf(_capacity <= (SIZE_MAX / sizeof(*((_array)->data))), "s_array_init :: Capacity overflow\n"); \
            (_array)->data = malloc(sizeof(*((_array)->data)) * _capacity); \
            (_array)->size = 0; \
            (_array)->capacity = _capacity; \
            s_assertf((_array)->data != NULL, "s_array_init :: Failed to allocate memory\n"); \
            memset((_array)->data, 0, sizeof(*((_array)->data)) * _capacity); \
        } else { \
            (_array)->data = NULL; \
            (_array)->size = 0; \
            (_array)->capacity = 0; \
        } \
    } while (0)

#define s_array_clear(_array) \
    do { \
        s_assertf((_array) != NULL, "s_array_clear :: Array is null\n"); \
        if ((_array)->data != NULL) { \
            free((_array)->data); \
        } \
        (_array)->data = NULL; \
        (_array)->size = 0; \
        (_array)->capacity = 0; \
    } while (0)

#define s_array_resize(_array, _new_capacity) \
    do { \
        s_assertf((_array) != NULL, "s_array_resize :: Array is null\n"); \
        sz _target_capacity = (sz)(_new_capacity); \
        if (_target_capacity == 0) { \
            if ((_array)->data != NULL) { \
                free((_array)->data); \
            } \
            (_array)->data = NULL; \
            (_array)->size = 0; \
            (_array)->capacity = 0; \
        } else { \
            s_assertf(_target_capacity <= (SIZE_MAX / sizeof(*((_array)->data))), "s_array_resize :: Capacity overflow\n"); \
            sz _old_capacity = (_array)->capacity; \
            void* _new_data = realloc((_array)->data, sizeof(*((_array)->data)) * _target_capacity); \
            s_assertf(_new_data != NULL, "s_array_resize :: Failed to allocate memory\n"); \
            (_array)->data = _new_data; \
            if (_target_capacity > _old_capacity) { \
                memset(&(_array)->data[_old_capacity], 0, sizeof(*((_array)->data)) * (_target_capacity - _old_capacity)); \
            } \
            (_array)->capacity = _target_capacity; \
            if ((_array)->size > _target_capacity) { \
                (_array)->size = _target_capacity; \
            } \
        } \
    } while (0)

#define s_array_increment(_array) \
    ( \
        s_array_require((_array) != NULL, "s_array_increment :: Array is null\n"), \
        s_array_require((_array)->data != NULL, "s_array_increment :: Array data is null\n"), \
        s_array_require((_array)->size < (_array)->capacity, "s_array_increment :: Array is full\n"), \
        &((_array)->data[(_array)->size++]) \
    )

#define s_array_add(_array, _value) \
    do { \
        s_assertf((_array) != NULL, "s_array_add :: Array is null\n"); \
        s_assertf((_array)->data != NULL, "s_array_add :: Array data is null\n"); \
        s_assertf((_array)->size < (_array)->capacity, "s_array_add :: Array is full\n"); \
        (_array)->data[(_array)->size++] = (_value); \
    } while (0)

#define s_array_add_many(_array, _type, ...) \
    do { \
        _type _values[] = { __VA_ARGS__ }; \
        sz _count = sizeof(_values) / sizeof(_values[0]); \
        s_assertf((_array) != NULL, "s_array_add_many :: Array is null\n"); \
        s_assertf((_array)->data != NULL, "s_array_add_many :: Array data is null\n"); \
        s_assertf(_count <= ((_array)->capacity - (_array)->size), "s_array_add_many :: Array is full\n"); \
        memcpy(&(_array)->data[(_array)->size], _values, sizeof(_values)); \
        (_array)->size += _count; \
    } while (0)

#define s_array_remove_at(_array, _index) \
    do { \
        s_assertf((_array) != NULL, "s_array_remove :: Array is null\n"); \
        s_assertf((_array)->data != NULL, "s_array_remove :: Array data is null\n"); \
        sz _idx = (sz)(_index); \
        s_assertf(_idx < (_array)->size, "s_array_remove :: Index out of bounds\n"); \
        memmove(&(_array)->data[_idx], &(_array)->data[_idx + 1], sizeof(*((_array)->data)) * ((_array)->size - _idx - 1)); \
        (_array)->size--; \
    } while (0)

#define s_array_remove(_array, _ptr) \
    do { \
        s_assertf((_array) != NULL, "s_array_remove :: Array is null\n"); \
        s_assertf((_array)->data != NULL, "s_array_remove :: Array data is null\n"); \
        s_foreach((_array), _it) { \
            if (s_array_get((_array), _it) == (_ptr)) { \
                s_array_remove_at((_array), _it); \
                break; \
            } \
        } \
    } while (0)

#define s_array_remove_last(_array) \
    do { \
        s_assertf((_array) != NULL, "s_array_remove_last :: Array is null\n"); \
        s_assertf((_array)->data != NULL, "s_array_remove_last :: Array data is null\n"); \
        s_assertf((_array)->size > 0, "s_array_remove_last :: Array is empty\n"); \
        (_array)->size--; \
    } while (0)

#define s_remove_if(_array, _type, _current_value, _condition) \
    do { \
        s_assertf((_array) != NULL, "s_remove_if :: Array is null\n"); \
        s_assertf((_array)->data != NULL, "s_remove_if :: Array data is null\n"); \
        s_foreach((_array), _it) { \
            _type* _current_value = &((_array)->data[_it]); \
            if (_condition) { \
                s_array_remove_at((_array), _it); \
                break; \
            } \
        } \
    } while (0)

#define s_remove_all(_array, _type, _current_value, _condition) \
    do { \
        s_assertf((_array) != NULL, "s_remove_all :: Array is null\n"); \
        s_assertf((_array)->data != NULL || (_array)->size == 0, "s_remove_all :: Array data is null\n"); \
        if ((_array)->size > 0) { \
            s_foreach_reverse((_array), _it) { \
                _type* _current_value = &((_array)->data[_it]); \
                if (_condition) { \
                    s_array_remove_at((_array), _it); \
                } \
            } \
        } \
    } while (0)

#endif // S_ARRAY_H
