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

#define s_array(_type, _array) \
    struct { \
        _type* data; \
        sz size; \
        sz capacity; \
    } _array; \

#define s_array_get(_array, _index) \
    &(_array)->data[_index]
    
#define s_array_get_size(_array) \
    (_array)->size

#define s_array_get_capacity(_array) \
    (_array)->capacity

#define s_foreach(_array, _it) \
    s_assertf((_array) != NULL, "s_foreach :: Array is null\n"); \
    for (sz _it = 0; _it < (_array)->size; _it++)

#define s_foreach_reverse(_array, _it) \
    s_assertf((_array) != NULL, "s_foreach_reverse :: Array is null\n"); \
    for (sz _it = (_array)->size; _it-- > 0;)

#define s_array_init(_array, _size) \
    s_assertf((_array) != NULL, "s_array_init :: Array is null\n"); \
    if (_size > 0) { \
        (_array)->data = malloc(sizeof(*((_array)->data)) * _size); \
        (_array)->size = 0; \
        (_array)->capacity = _size; \
        s_assertf((_array)->data != NULL, "s_array_init :: Failed to allocate memory\n"); \
        memset((_array)->data, 0, sizeof(*((_array)->data)) * _size); \
    } else { \
        (_array)->data = NULL; \
        (_array)->size = 0; \
        (_array)->capacity = 0; \
    }

#define s_array_clear(_array) \
    s_assertf((_array) != NULL, "s_array_clear :: Array is null\n"); \
    if ((_array)->data != NULL) { \
        free((_array)->data); \
    } \
    (_array)->data = NULL; \
    (_array)->size = 0; \
    (_array)->capacity = 0; \

#define s_array_increment(_array) \
    &((_array)->data[(_array)->size++])

#define s_array_add(_array, _value) \
    s_assertf((_array) != NULL, "s_array_add :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_add :: Array data is null\n"); \
    s_assertf((_array)->size < (_array)->capacity, "s_array_add :: Array is full\n"); \
    (_array)->data[(_array)->size++] = _value;

#define s_array_remove_at(_array, _index) \
    s_assertf((_array) != NULL, "s_array_remove :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_remove :: Array data is null\n"); \
    s_assertf((_index) >= 0 && (_index) < (_array)->size, "s_array_remove :: Index out of bounds\n"); \
    memmove(&(_array)->data[_index], &(_array)->data[_index + 1], sizeof(*((_array)->data)) * ((_array)->size - _index - 1)); \
    (_array)->size--; \

#define s_array_remove(_array, _value) \
    s_assertf((_array) != NULL, "s_array_remove :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_remove :: Array data is null\n"); \
    s_foreach((_array), _it) { \
        if (s_array_get((_array), _it) == _value) { \
            s_array_remove_at((_array), _it); \
            break; \
        } \
    }

#define s_array_remove_last(_array) \
    s_assertf((_array) != NULL, "s_array_remove_last :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_remove_last :: Array data is null\n"); \
    s_assertf((_array)->size > 0, "s_array_remove_last :: Array is empty\n"); \
    (_array)->size--; \

#define s_remove_if(_array_type, _array, _current_value, _condition) \
    s_foreach(_array_type, (_array), _it) { \
        if (_condition) { \
            _array_type##_remove_at((_array), _it); \
            break; \
        } \
    }

#define s_remove_all(_array_type, _array, _type, _current_value, _condition) \
    if (_array_type##_get_size(_array) <= 0) { \
        return; \
    }  \
    s_foreach_reverse(_array_type, (_array), _it) { \
        _type* _current_value = _array_type##_get((_array), _it); \
        if (_condition) { \
            _array_type##_remove_at((_array), _it); \
        } \
    }

#endif // S_ARRAY_H
