// Syphax - Ougi Washi

#ifndef S_ARRAY_H
#define S_ARRAY_H

#include "s_types.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

// This approach is inspired by arena allocators but per array instead of being block-based to avoid fragmentation
// while offering a simple array handling interface.

// Old implementation of arrays (S_DEFINE_ARRAY)
#define S_DEFINE_ARRAY(_type, _array, _size) \
    typedef struct { \
        _type data[_size]; \
        sz size; \
    } _array; \
    static void _array##_init(_array* array) { \
        memset(array, 0, sizeof(_type) * _size); \
        array->size = 0; \
    } \
    static _type* _array##_increment(_array* array) { \
        if (array->size == _size) { \
            return NULL; \
        } \
        array->size++; \
        return &array->data[array->size - 1]; \
    } \
    static _type* _array##_add(_array* array, _type value) { \
        s_assertf(array->size < _size, "sw_array_add :: Array is full\n"); \
        _type* new_element = _array##_increment(array); \
        *new_element = value; \
        return new_element; \
    } \
    static sz _array##_find(_array* array, _type* value) { \
        for (sz i = 0; i < array->size; i++) { \
            if (&array->data[i] == value) { \
                return i; \
            } \
        } \
        return -1; \
    } \
    static sz _array##_find_last(_array* array, _type* value) { \
        sz index = array->size; \
        while (index > 0 && index <= array->size) { \
            index--; \
            if (&array->data[index] == value) { \
                return index; \
            } \
        } \
        return -1; \
    } \
    static void _array##_remove_at(_array* array, const size_t index) { \
        if (index >= array->size) return; \
        memmove(&array->data[index], &array->data[index + 1], sizeof(_type) * (array->size - index - 1)); \
        array->size--; \
    } \
    static void _array##_remove(_array* array, _type* value) { \
        const sz index = _array##_find_last(array, value); \
        if (index >= 0) { \
            _array##_remove_at(array, index); \
        } \
    } \
    static _type* _array##_get(_array* array, const sz index) { \
        s_assertf(index >= 0 && index < array->size, "sw_array_get :: Index out of bounds %zu\n", index); \
        return &array->data[index]; \
    } \
    static _type* _array##_get_last(_array* array) { \
        s_assertf(array->size > 0, "sw_array_get_last :: Array is empty\n"); \
        return &array->data[array->size - 1]; \
    } \
    static void _array##_set(_array* array, const sz index, _type* value) { \
        s_assertf(index < array->size, "sw_array_set :: Index out of bounds %zu\n", index); \
        array->data[index] = *value; \
    } \
    static void _array##_clear(_array* array) { \
        memset(array->data, 0, sizeof(_type) * array->size); \
        array->size = 0; \
    } \
    static sz _array##_get_size(const _array* array) { \
        return array->size; \
    } 

// New implementation of arrays
#define s_array(_type, _array) \
    struct { \
        _type* data; \
        sz size; \
        sz capacity; \
    } _array; \

#define s_array_init(_array, _size) \
    s_assertf((_array) != NULL, "s_array_init :: Array is null\n"); \
    if ((_array)->data != NULL) { \
        free((_array)->data); \
    } \
    (_array)->data = malloc(sizeof(_type) * _size); \
    (_array)->size = 0; \
    (_array)->capacity = _size; \
    s_assertf((_array)->data != NULL, "s_array_init :: Failed to allocate memory\n");

#define s_array_clear(_array) \
    s_assertf((_array) != NULL, "s_array_clear :: Array is null\n"); \
    if ((_array)->data != NULL) { \
        free((_array)->data); \
    } \
    (_array)->data = NULL; \
    (_array)->size = 0; \
    (_array)->capacity = 0; \

#define s_array_increment(_array) \
    s_assertf((_array) != NULL, "s_array_increment :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_increment :: Array data is null\n"); \
    s_assertf((_array)->size < (_array)->capacity, "s_array_increment :: Array is full\n"); \
    (_array)->data[(_array)->size++]

#define s_array_add(_array, _value) \
    s_assertf((_array) != NULL, "s_array_add :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_add :: Array data is null\n"); \
    s_assertf((_array)->size < (_array)->capacity, "s_array_add :: Array is full\n"); \
    (_array)->data[(_array)->size++] = _value;

#define s_array_remove(_array, _index) \
    s_assertf((_array) != NULL, "s_array_remove :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_remove :: Array data is null\n"); \
    s_assertf((_index) >= 0 && (_index) < (_array)->size, "s_array_remove :: Index out of bounds\n"); \
    memmove(&(_array)->data[_index], &(_array)->data[_index + 1], sizeof(_type) * ((_array)->size - _index - 1)); \
    (_array)->size--; \

#define s_array_remove_last(_array) \
    s_assertf((_array) != NULL, "s_array_remove_last :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_remove_last :: Array data is null\n"); \
    s_assertf((_array)->size > 0, "s_array_remove_last :: Array is empty\n"); \
    (_array)->size--; \

#define s_array_get(_array, _index) \
    s_assertf((_array) != NULL, "s_array_get :: Array is null\n"); \
    s_assertf((_array)->data != NULL, "s_array_get :: Array data is null\n"); \
    s_assertf((_index) >= 0 && (_index) < (_array)->size, "s_array_get :: Index out of bounds\n"); \
    &(_array)->data[_index]
    
#define s_array_get_size(_array) \
    s_assertf((_array) != NULL, "s_array_get_size :: Array is null\n"); \
    (_array)->size

#define s_foreach(_array, _it) \
    s_assertf((_array) != NULL, "s_foreach :: Array is null\n"); \
    for (sz _it = 0; _it < (_array)->size; _it++)

#define s_foreach_reverse(_array, _it) \
    s_assertf((_array) != NULL, "s_foreach_reverse :: Array is null\n"); \
    for (sz _it = (_array)->size; _it-- > 0;)

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
