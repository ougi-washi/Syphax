/**
 * Syphax - Ougi Washi
 *
 * Simple JSON reader/writer for C.
 *
 * MIT License
 * Copyright (c) 2025-2026 Jed - Ougi Washi https://github.com/ougi-washi/Syphax
 */

#ifndef S_JSON_H
#define S_JSON_H

#include "s_types.h"
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <errno.h>

#ifndef S_JSON_MAX_DEPTH
#define S_JSON_MAX_DEPTH 256
#endif

typedef enum {
    S_JSON_NULL = 0,
    S_JSON_BOOL,
    S_JSON_NUMBER,
    S_JSON_STRING,
    S_JSON_ARRAY,
    S_JSON_OBJECT
} s_json_type;

typedef struct s_json s_json;

typedef struct {
    s_json** items;
    sz count;
    sz capacity;
} s_json_children;

struct s_json {
    s_json_type type;
    char* name;
    union {
        b8 boolean;
        f64 number;
        char* string;
        s_json_children children;
    } as;
};

typedef enum {
    S_JSON_OK = 0,
    S_JSON_ERR_OOM,
    S_JSON_ERR_EOF,
    S_JSON_ERR_UNEXPECTED_CHAR,
    S_JSON_ERR_INVALID_STRING,
    S_JSON_ERR_INVALID_NUMBER,
    S_JSON_ERR_TRAILING_CHAR,
    S_JSON_ERR_DEPTH
} s_json_error_code;

typedef struct {
    s_json_error_code code;
    sz offset;
    sz line;
    sz column;
} s_json_error;

typedef struct {
    char* data;
    sz size;
    sz capacity;
    b8 grow;
    b8 failed;
    i32 number_precision;
} s_json_writer;

#if !defined(SYPHAX_STATIC)
const char* s_json_error_string(s_json_error_code _code);
char* s_json_strdup(const char* _str);
s_json* s_json_new(s_json_type _type, const char* _name);
s_json* s_json_new_take(s_json_type _type, char* _name);
void s_json_set_name_take(s_json* _node, char* _name);
void s_json_set_name(s_json* _node, const char* _name);
s_json* s_json_null(const char* _name);
s_json* s_json_bool(const char* _name, b8 _value);
s_json* s_json_int(const char* _name, i64 _value);
s_json* s_json_num(const char* _name, f64 _value);
s_json* s_json_str_take(const char* _name, char* _value);
s_json* s_json_str(const char* _name, const char* _value);
b8 s_json_children_reserve(s_json_children* _children, sz _cap);
b8 s_json_add(s_json* _parent, s_json* _child);
s_json* s_json_array_n(const char* _name, sz _count, s_json** _items);
s_json* s_json_object_n(const char* _name, sz _count, s_json** _items);
s_json* s_json_get(const s_json* _obj, const char* _key);
s_json* s_json_at(const s_json* _arr, sz _index);
sz s_json_count(const s_json* _node);
b8 s_json_is_null(const s_json* _node);
b8 s_json_is_bool(const s_json* _node);
b8 s_json_is_number(const s_json* _node);
b8 s_json_is_string(const s_json* _node);
b8 s_json_is_array(const s_json* _node);
b8 s_json_is_object(const s_json* _node);
const char* s_json_as_str(const s_json* _node);
b8 s_json_as_bool(const s_json* _node, b8* _out);
b8 s_json_as_num(const s_json* _node, f64* _out);
b8 s_json_as_i64(const s_json* _node, i64* _out);
b8 s_json_object_add(s_json* _obj, const char* _key, s_json* _child);
b8 s_json_array_add(s_json* _arr, s_json* _child);
void s_json_free(s_json* _node);
b8 s_json_object_add_node(s_json* _obj, const char* _key, s_json* _child);
b8 s_json_array_add_node(s_json* _arr, s_json* _child);
b8 s_json_object_add_null(s_json* _obj, const char* _key);
b8 s_json_object_add_bool(s_json* _obj, const char* _key, b8 _value);
b8 s_json_object_add_int(s_json* _obj, const char* _key, i64 _value);
b8 s_json_object_add_num(s_json* _obj, const char* _key, f64 _value);
b8 s_json_object_add_str(s_json* _obj, const char* _key, const char* _value);
b8 s_json_array_add_null(s_json* _arr);
b8 s_json_array_add_bool(s_json* _arr, b8 _value);
b8 s_json_array_add_int(s_json* _arr, i64 _value);
b8 s_json_array_add_num(s_json* _arr, f64 _value);
b8 s_json_array_add_str(s_json* _arr, const char* _value);
s_json* s_json_get_range(const s_json* _obj, const char* _key, sz _key_len);
s_json* s_json_get_path(const s_json* _root, const char* _path);
const char* s_json_get_path_str(const s_json* _root, const char* _path);
b8 s_json_get_path_bool(const s_json* _root, const char* _path, b8* _out);
b8 s_json_get_path_num(const s_json* _root, const char* _path, f64* _out);
b8 s_json_get_path_i64(const s_json* _root, const char* _path, i64* _out);
b8 s_json_writer_reserve(s_json_writer* _wr, sz _need);
b8 s_json_writer_append(s_json_writer* _wr, const char* _data, sz _len);
b8 s_json_writer_append_char(s_json_writer* _wr, char _c);
b8 s_json_writer_append_escaped(s_json_writer* _wr, const char* _str);
b8 s_json_writer_append_number(s_json_writer* _wr, f64 _value);
b8 s_json_write_value(s_json_writer* _wr, const s_json* _node);
b8 s_json_write_array(s_json_writer* _wr, const s_json* _node);
b8 s_json_write_object(s_json_writer* _wr, const s_json* _node);
char* s_json_stringify_precision(const s_json* _node, i32 _number_precision);
char* s_json_stringify(const s_json* _node);
sz s_json_write_precision(const s_json* _node, char* _buffer, sz _capacity, i32 _number_precision);
sz s_json_write(const s_json* _node, char* _buffer, sz _capacity);
s_json* s_json_parse_with_error(const char* _text, s_json_error* _out_err);
s_json* s_json_parse(const char* _text);
#endif

#define s_json_array_empty(_name) s_json_array_n((_name), 0, NULL)
#define s_json_object_empty(_name) s_json_object_n((_name), 0, NULL)

#define s_json_array(_name, ...) \
    s_json_array_n((_name), (sz)(sizeof((s_json*[]){__VA_ARGS__}) / sizeof(s_json*)), (s_json*[]){__VA_ARGS__})

#define s_json_object(_name, ...) \
    s_json_object_n((_name), (sz)(sizeof((s_json*[]){__VA_ARGS__}) / sizeof(s_json*)), (s_json*[]){__VA_ARGS__})

#if defined(SYPHAX_STATIC) || defined(SYPHAX_IMPLEMENTATION)
#if defined(SYPHAX_STATIC)
#define S_JSON_DEF static inline
#define S_JSON_PRIV static inline
#else
#define S_JSON_DEF
#define S_JSON_PRIV static
#endif

S_JSON_DEF const char* s_json_error_string(s_json_error_code _code) {
    switch (_code) {
        case S_JSON_OK: return "ok";
        case S_JSON_ERR_OOM: return "out of memory";
        case S_JSON_ERR_EOF: return "unexpected end of input";
        case S_JSON_ERR_UNEXPECTED_CHAR: return "unexpected character";
        case S_JSON_ERR_INVALID_STRING: return "invalid string";
        case S_JSON_ERR_INVALID_NUMBER: return "invalid number";
        case S_JSON_ERR_TRAILING_CHAR: return "trailing characters";
        case S_JSON_ERR_DEPTH: return "maximum nesting depth exceeded";
        default: return "unknown JSON error";
    }
}

S_JSON_DEF char* s_json_strdup(const char* _str) {
    if (_str == NULL) return NULL;
    const sz len = strlen(_str);
    char* out = (char*)s_malloc(len + 1);
    if (out == NULL) return NULL;
    memcpy(out, _str, len + 1);
    return out;
}

S_JSON_DEF s_json* s_json_new_take(s_json_type _type, char* _name) {
    s_json* node = (s_json*)s_malloc(sizeof(s_json));
    if (node == NULL) {
        s_free(_name);
        return NULL;
    }
    memset(node, 0, sizeof(*node));
    node->type = _type;
    node->name = _name;
    return node;
}

S_JSON_DEF s_json* s_json_new(s_json_type _type, const char* _name) {
    char* name = NULL;
    if (_name != NULL) {
        name = s_json_strdup(_name);
        if (name == NULL) return NULL;
    }
    return s_json_new_take(_type, name);
}

S_JSON_DEF void s_json_set_name_take(s_json* _node, char* _name) {
    if (_node == NULL) {
        s_free(_name);
        return;
    }
    if (_node->name != NULL) s_free(_node->name);
    _node->name = _name;
}

S_JSON_DEF void s_json_set_name(s_json* _node, const char* _name) {
    if (_node == NULL) return;
    char* name_copy = NULL;
    if (_name != NULL) {
        name_copy = s_json_strdup(_name);
        if (name_copy == NULL) return;
    }
    s_free(_node->name);
    _node->name = name_copy;
}

S_JSON_DEF s_json* s_json_null(const char* _name) {
    return s_json_new(S_JSON_NULL, _name);
}

S_JSON_DEF s_json* s_json_bool(const char* _name, b8 _value) {
    s_json* node = s_json_new(S_JSON_BOOL, _name);
    if (node == NULL) return NULL;
    node->as.boolean = _value;
    return node;
}

S_JSON_DEF s_json* s_json_int(const char* _name, i64 _value) {
    s_json* node = s_json_new(S_JSON_NUMBER, _name);
    if (node == NULL) return NULL;
    node->as.number = (f64)_value;
    return node;
}

S_JSON_DEF s_json* s_json_num(const char* _name, f64 _value) {
    s_json* node = s_json_new(S_JSON_NUMBER, _name);
    if (node == NULL) return NULL;
    node->as.number = _value;
    return node;
}

S_JSON_DEF s_json* s_json_str_take(const char* _name, char* _value) {
    s_json* node = s_json_new(S_JSON_STRING, _name);
    if (node == NULL) {
        s_free(_value);
        return NULL;
    }
    node->as.string = _value;
    return node;
}

S_JSON_DEF s_json* s_json_str(const char* _name, const char* _value) {
    s_json* node = s_json_new(S_JSON_STRING, _name);
    if (node == NULL) return NULL;
    node->as.string = s_json_strdup(_value ? _value : "");
    if (node->as.string == NULL) {
        s_free(node->name);
        s_free(node);
        return NULL;
    }
    return node;
}

S_JSON_DEF b8 s_json_children_reserve(s_json_children* _children, sz _cap) {
    if (_children == NULL) return false;
    if (_cap <= _children->capacity) return true;
    sz new_cap = _children->capacity == 0 ? 4 : _children->capacity;
    while (new_cap < _cap) {
        if (new_cap > SIZE_MAX / 2) {
            new_cap = _cap;
            break;
        }
        new_cap *= 2;
    }
    if (new_cap > SIZE_MAX / sizeof(s_json*)) return false;
    s_json** next = (s_json**)s_realloc(_children->items, new_cap * sizeof(s_json*));
    if (next == NULL) return false;
    _children->items = next;
    _children->capacity = new_cap;
    return true;
}

S_JSON_DEF b8 s_json_add(s_json* _parent, s_json* _child) {
    if (_parent == NULL || _child == NULL) return false;
    if (!(_parent->type == S_JSON_ARRAY || _parent->type == S_JSON_OBJECT)) return false;
    if (_parent->as.children.count == SIZE_MAX) return false;
    if (!s_json_children_reserve(&_parent->as.children, _parent->as.children.count + 1)) return false;
    _parent->as.children.items[_parent->as.children.count++] = _child;
    return true;
}

S_JSON_PRIV s_json* s_json_container_n(s_json_type _type, const char* _name, sz _count, s_json** _items) {
    s_json* node = s_json_new(_type, _name);
    if (node == NULL) return NULL;
    if (_count > 0) {
        if (_items == NULL || _count > SIZE_MAX / sizeof(s_json*)) {
            s_free(node->name);
            s_free(node);
            return NULL;
        }
        node->as.children.items = (s_json**)s_malloc(sizeof(s_json*) * _count);
        if (node->as.children.items == NULL) {
            s_free(node->name);
            s_free(node);
            return NULL;
        }
        memcpy(node->as.children.items, _items, sizeof(s_json*) * _count);
        node->as.children.count = _count;
        node->as.children.capacity = _count;
    }
    return node;
}

S_JSON_DEF s_json* s_json_array_n(const char* _name, sz _count, s_json** _items) {
    return s_json_container_n(S_JSON_ARRAY, _name, _count, _items);
}

S_JSON_DEF s_json* s_json_object_n(const char* _name, sz _count, s_json** _items) {
    return s_json_container_n(S_JSON_OBJECT, _name, _count, _items);
}

S_JSON_DEF s_json* s_json_get(const s_json* _obj, const char* _key) {
    if (_obj == NULL || _obj->type != S_JSON_OBJECT || _key == NULL) return NULL;
    for (sz i = 0; i < _obj->as.children.count; i++) {
        s_json* child = _obj->as.children.items[i];
        if (child != NULL && child->name != NULL && strcmp(child->name, _key) == 0) return child;
    }
    return NULL;
}

S_JSON_DEF s_json* s_json_at(const s_json* _arr, sz _index) {
    if (_arr == NULL || _arr->type != S_JSON_ARRAY) return NULL;
    if (_index >= _arr->as.children.count) return NULL;
    return _arr->as.children.items[_index];
}

S_JSON_DEF sz s_json_count(const s_json* _node) {
    if (_node == NULL || !(_node->type == S_JSON_ARRAY || _node->type == S_JSON_OBJECT)) return 0;
    return _node->as.children.count;
}

S_JSON_DEF b8 s_json_is_null(const s_json* _node) {
    return _node != NULL && _node->type == S_JSON_NULL;
}

S_JSON_DEF b8 s_json_is_bool(const s_json* _node) {
    return _node != NULL && _node->type == S_JSON_BOOL;
}

S_JSON_DEF b8 s_json_is_number(const s_json* _node) {
    return _node != NULL && _node->type == S_JSON_NUMBER;
}

S_JSON_DEF b8 s_json_is_string(const s_json* _node) {
    return _node != NULL && _node->type == S_JSON_STRING;
}

S_JSON_DEF b8 s_json_is_array(const s_json* _node) {
    return _node != NULL && _node->type == S_JSON_ARRAY;
}

S_JSON_DEF b8 s_json_is_object(const s_json* _node) {
    return _node != NULL && _node->type == S_JSON_OBJECT;
}

S_JSON_DEF const char* s_json_as_str(const s_json* _node) {
    if (!s_json_is_string(_node)) return NULL;
    return _node->as.string != NULL ? _node->as.string : "";
}

S_JSON_DEF b8 s_json_as_bool(const s_json* _node, b8* _out) {
    if (!s_json_is_bool(_node) || _out == NULL) return false;
    *_out = _node->as.boolean;
    return true;
}

S_JSON_DEF b8 s_json_as_num(const s_json* _node, f64* _out) {
    if (!s_json_is_number(_node) || _out == NULL) return false;
    *_out = _node->as.number;
    return true;
}

S_JSON_DEF b8 s_json_as_i64(const s_json* _node, i64* _out) {
    f64 number;
    if (!s_json_as_num(_node, &number) || _out == NULL) return false;
    if (!isfinite(number) || number < (f64)INT64_MIN || number >= 9223372036854775808.0) return false;
    if (trunc(number) != number) return false;
    *_out = (i64)number;
    return true;
}

S_JSON_DEF b8 s_json_object_add(s_json* _obj, const char* _key, s_json* _child) {
    char* key_copy;
    if (_obj == NULL || _obj->type != S_JSON_OBJECT || _key == NULL || _child == NULL) return false;
    if (_obj->as.children.count == SIZE_MAX) return false;
    key_copy = s_json_strdup(_key);
    if (key_copy == NULL) return false;
    if (!s_json_children_reserve(&_obj->as.children, _obj->as.children.count + 1)) {
        s_free(key_copy);
        return false;
    }
    s_json_set_name_take(_child, key_copy);
    _obj->as.children.items[_obj->as.children.count++] = _child;
    return true;
}

S_JSON_DEF b8 s_json_array_add(s_json* _arr, s_json* _child) {
    if (_arr == NULL || _arr->type != S_JSON_ARRAY || _child == NULL) return false;
    return s_json_add(_arr, _child);
}

S_JSON_DEF void s_json_free(s_json* _node);

S_JSON_DEF b8 s_json_object_add_node(s_json* _obj, const char* _key, s_json* _child) {
    if (!s_json_object_add(_obj, _key, _child)) {
        s_json_free(_child);
        return false;
    }
    return true;
}

S_JSON_DEF b8 s_json_array_add_node(s_json* _arr, s_json* _child) {
    if (!s_json_array_add(_arr, _child)) {
        s_json_free(_child);
        return false;
    }
    return true;
}

S_JSON_DEF b8 s_json_object_add_null(s_json* _obj, const char* _key) {
    return s_json_object_add_node(_obj, _key, s_json_null(NULL));
}

S_JSON_DEF b8 s_json_object_add_bool(s_json* _obj, const char* _key, b8 _value) {
    return s_json_object_add_node(_obj, _key, s_json_bool(NULL, _value));
}

S_JSON_DEF b8 s_json_object_add_int(s_json* _obj, const char* _key, i64 _value) {
    return s_json_object_add_node(_obj, _key, s_json_int(NULL, _value));
}

S_JSON_DEF b8 s_json_object_add_num(s_json* _obj, const char* _key, f64 _value) {
    return s_json_object_add_node(_obj, _key, s_json_num(NULL, _value));
}

S_JSON_DEF b8 s_json_object_add_str(s_json* _obj, const char* _key, const char* _value) {
    return s_json_object_add_node(_obj, _key, s_json_str(NULL, _value));
}

S_JSON_DEF b8 s_json_array_add_null(s_json* _arr) {
    return s_json_array_add_node(_arr, s_json_null(NULL));
}

S_JSON_DEF b8 s_json_array_add_bool(s_json* _arr, b8 _value) {
    return s_json_array_add_node(_arr, s_json_bool(NULL, _value));
}

S_JSON_DEF b8 s_json_array_add_int(s_json* _arr, i64 _value) {
    return s_json_array_add_node(_arr, s_json_int(NULL, _value));
}

S_JSON_DEF b8 s_json_array_add_num(s_json* _arr, f64 _value) {
    return s_json_array_add_node(_arr, s_json_num(NULL, _value));
}

S_JSON_DEF b8 s_json_array_add_str(s_json* _arr, const char* _value) {
    return s_json_array_add_node(_arr, s_json_str(NULL, _value));
}

S_JSON_DEF s_json* s_json_get_range(const s_json* _obj, const char* _key, sz _key_len) {
    if (_obj == NULL || _obj->type != S_JSON_OBJECT || _key == NULL || _key_len == 0) return NULL;
    for (sz i = 0; i < _obj->as.children.count; i++) {
        s_json* child = _obj->as.children.items[i];
        if (child != NULL && child->name != NULL && strlen(child->name) == _key_len && strncmp(child->name, _key, _key_len) == 0) return child;
    }
    return NULL;
}

S_JSON_DEF s_json* s_json_get_path(const s_json* _root, const char* _path) {
    const char* cursor;
    const s_json* node = _root;

    if (_root == NULL || _path == NULL || _path[0] == '\0') return NULL;
    cursor = _path;
    if (*cursor == '$') {
        ++cursor;
        if (*cursor == '\0') return (s_json*)node;
    }

    while (*cursor != '\0') {
        if (*cursor == '.') {
            ++cursor;
            if (*cursor == '\0' || *cursor == '[') return NULL;
        }
        if (*cursor == '[') {
            sz index = 0;
            ++cursor;
            if (*cursor < '0' || *cursor > '9') return NULL;
            while (*cursor >= '0' && *cursor <= '9') {
                const sz digit = (sz)(*cursor - '0');
                if (index > (SIZE_MAX - digit) / 10) return NULL;
                index = (index * 10) + digit;
                ++cursor;
            }
            if (*cursor != ']') return NULL;
            ++cursor;
            node = s_json_at(node, index);
            if (node == NULL) return NULL;
            continue;
        }
        const char* key_begin = cursor;
        while (*cursor != '\0' && *cursor != '.' && *cursor != '[') ++cursor;
        if (cursor == key_begin) return NULL;
        node = s_json_get_range(node, key_begin, (sz)(cursor - key_begin));
        if (node == NULL) return NULL;
    }

    return (s_json*)node;
}

S_JSON_DEF const char* s_json_get_path_str(const s_json* _root, const char* _path) {
    return s_json_as_str(s_json_get_path(_root, _path));
}

S_JSON_DEF b8 s_json_get_path_bool(const s_json* _root, const char* _path, b8* _out) {
    return s_json_as_bool(s_json_get_path(_root, _path), _out);
}

S_JSON_DEF b8 s_json_get_path_num(const s_json* _root, const char* _path, f64* _out) {
    return s_json_as_num(s_json_get_path(_root, _path), _out);
}

S_JSON_DEF b8 s_json_get_path_i64(const s_json* _root, const char* _path, i64* _out) {
    return s_json_as_i64(s_json_get_path(_root, _path), _out);
}

S_JSON_DEF void s_json_free(s_json* _node) {
    if (_node == NULL) return;
    if (_node->type == S_JSON_STRING && _node->as.string != NULL) {
        s_free(_node->as.string);
    }
    if (_node->type == S_JSON_ARRAY || _node->type == S_JSON_OBJECT) {
        for (sz i = 0; i < _node->as.children.count; i++) {
            s_json_free(_node->as.children.items[i]);
        }
        s_free(_node->as.children.items);
    }
    s_free(_node->name);
    s_free(_node);
}

S_JSON_PRIV b8 s_json_buffer_reserve(char** buf, sz* cap, sz need) {
    if (buf == NULL || cap == NULL) return false;
    if (need <= *cap && (need == 0 || *buf != NULL)) return true;
    sz new_cap = *cap == 0 ? 16 : *cap;
    while (new_cap < need) {
        if (new_cap > SIZE_MAX / 2) {
            new_cap = need;
            break;
        }
        new_cap *= 2;
    }
    char* next = (char*)s_realloc(*buf, new_cap);
    if (next == NULL) return false;
    *buf = next;
    *cap = new_cap;
    return true;
}

S_JSON_DEF b8 s_json_writer_reserve(s_json_writer* _wr, sz _need) {
    if (_wr == NULL) return false;
    if (_wr->failed) return false;
    if (_need <= _wr->capacity && (_need == 0 || _wr->data != NULL)) return true;
    if (!_wr->grow) {
        _wr->failed = true;
        return false;
    }
    if (!s_json_buffer_reserve(&_wr->data, &_wr->capacity, _need)) {
        _wr->failed = true;
        return false;
    }
    return true;
}

S_JSON_DEF b8 s_json_writer_append(s_json_writer* _wr, const char* _data, sz _len) {
    if (_wr == NULL || (_len > 0 && _data == NULL)) return false;
    if (_wr->size > SIZE_MAX - _len || _wr->size + _len > SIZE_MAX - 1) {
        _wr->failed = true;
        return false;
    }
    if (!s_json_writer_reserve(_wr, _wr->size + _len + 1)) return false;
    if (_len > 0) memcpy(_wr->data + _wr->size, _data, _len);
    _wr->size += _len;
    _wr->data[_wr->size] = '\0';
    return true;
}

S_JSON_DEF b8 s_json_writer_append_char(s_json_writer* _wr, char _c) {
    return s_json_writer_append(_wr, &_c, 1);
}

S_JSON_PRIV b8 s_json_cstring_utf8_len(const unsigned char* _p, sz* _out_len) {
    u32 cp = 0;
    sz extra = 0;
    unsigned char first;
    if (_p == NULL || _out_len == NULL) return false;
    first = _p[0];
    if (first <= 0x7F) {
        *_out_len = 1;
        return true;
    } else if (first >= 0xC2 && first <= 0xDF) {
        cp = first & 0x1Fu;
        extra = 1;
    } else if (first >= 0xE0 && first <= 0xEF) {
        cp = first & 0x0Fu;
        extra = 2;
    } else if (first >= 0xF0 && first <= 0xF4) {
        cp = first & 0x07u;
        extra = 3;
    } else {
        return false;
    }
    for (sz i = 0; i < extra; ++i) {
        unsigned char c = _p[i + 1];
        if (c == '\0' || (c & 0xC0u) != 0x80u) return false;
        cp = (cp << 6) | (u32)(c & 0x3Fu);
    }
    if ((extra == 2 && cp < 0x800u) ||
        (extra == 3 && cp < 0x10000u) ||
        cp > 0x10FFFFu ||
        (cp >= 0xD800u && cp <= 0xDFFFu)) {
        return false;
    }
    *_out_len = extra + 1;
    return true;
}

S_JSON_DEF b8 s_json_writer_append_escaped(s_json_writer* _wr, const char* _str) {
    if (_wr == NULL || _str == NULL) return false;
    if (!s_json_writer_append_char(_wr, '"')) return false;
    for (const unsigned char* p = (const unsigned char*)_str; *p; ) {
        const unsigned char c = *p;
        switch (c) {
            case '"': if (!s_json_writer_append(_wr, "\\\"", 2)) return false; p++; break;
            case '\\': if (!s_json_writer_append(_wr, "\\\\", 2)) return false; p++; break;
            case '\b': if (!s_json_writer_append(_wr, "\\b", 2)) return false; p++; break;
            case '\f': if (!s_json_writer_append(_wr, "\\f", 2)) return false; p++; break;
            case '\n': if (!s_json_writer_append(_wr, "\\n", 2)) return false; p++; break;
            case '\r': if (!s_json_writer_append(_wr, "\\r", 2)) return false; p++; break;
            case '\t': if (!s_json_writer_append(_wr, "\\t", 2)) return false; p++; break;
            default:
                if (c < 0x20) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    if (!s_json_writer_append(_wr, buf, 6)) return false;
                    p++;
                } else if (c < 0x80) {
                    if (!s_json_writer_append_char(_wr, (char)c)) return false;
                    p++;
                } else {
                    sz utf8_len = 0;
                    if (!s_json_cstring_utf8_len(p, &utf8_len)) return false;
                    if (!s_json_writer_append(_wr, (const char*)p, utf8_len)) return false;
                    p += utf8_len;
                }
                break;
        }
    }
    return s_json_writer_append_char(_wr, '"');
}

S_JSON_PRIV b8 s_json_number_text_is_zero(const char* _text) {
    if (_text == NULL || _text[0] == '\0') return false;
    for (const char* p = _text; *p; ++p) {
        if (*p == '-') continue;
        if (*p == '.') continue;
        if (*p != '0') return false;
    }
    return true;
}

S_JSON_PRIV b8 s_json_number_text_has_only_zero_fraction(const char* _text) {
    const char* dot = strchr(_text, '.');
    if (dot == NULL) return false;
    for (const char* p = dot + 1; *p; ++p) {
        if (*p != '0') return false;
    }
    return true;
}

S_JSON_PRIV void s_json_number_text_strip_negative_zero(char* _text) {
    if (_text == NULL || _text[0] != '-') return;
    if (!s_json_number_text_is_zero(_text + 1)) return;
    memmove(_text, _text + 1, strlen(_text));
}

S_JSON_DEF b8 s_json_writer_append_number(s_json_writer* _wr, f64 _value) {
    char buf[128];
    int len = 0;
    if (_wr == NULL || !isfinite(_value)) return false;
    if (_wr->number_precision >= 0) {
        const i32 precision = _wr->number_precision > 15 ? 15 : _wr->number_precision;
        len = snprintf(buf, sizeof(buf), "%.*f", precision, _value);
        if (len <= 0 || (sz)len >= sizeof(buf)) return false;
        s_json_number_text_strip_negative_zero(buf);
        if (s_json_number_text_has_only_zero_fraction(buf)) {
            len = snprintf(buf, sizeof(buf), "%.0f", _value);
            if (len <= 0 || (sz)len >= sizeof(buf)) return false;
            s_json_number_text_strip_negative_zero(buf);
        }
    } else {
        len = snprintf(buf, sizeof(buf), "%.17g", _value);
        if (len <= 0 || (sz)len >= sizeof(buf)) return false;
    }
    return s_json_writer_append(_wr, buf, (sz)len);
}

S_JSON_DEF b8 s_json_write_value(s_json_writer* _wr, const s_json* _node);

S_JSON_DEF b8 s_json_write_array(s_json_writer* _wr, const s_json* _node) {
    if (_wr == NULL || _node == NULL || _node->type != S_JSON_ARRAY) return false;
    if (!s_json_writer_append_char(_wr, '[')) return false;
    for (sz i = 0; i < _node->as.children.count; i++) {
        if (i > 0 && !s_json_writer_append_char(_wr, ',')) return false;
        if (!s_json_write_value(_wr, _node->as.children.items[i])) return false;
    }
    return s_json_writer_append_char(_wr, ']');
}

S_JSON_DEF b8 s_json_write_object(s_json_writer* _wr, const s_json* _node) {
    if (_wr == NULL || _node == NULL || _node->type != S_JSON_OBJECT) return false;
    if (!s_json_writer_append_char(_wr, '{')) return false;
    for (sz i = 0; i < _node->as.children.count; i++) {
        s_json* child = _node->as.children.items[i];
        if (i > 0 && !s_json_writer_append_char(_wr, ',')) return false;
        if (child == NULL || child->name == NULL) return false;
        if (!s_json_writer_append_escaped(_wr, child->name)) return false;
        if (!s_json_writer_append_char(_wr, ':')) return false;
        if (!s_json_write_value(_wr, child)) return false;
    }
    return s_json_writer_append_char(_wr, '}');
}

S_JSON_DEF b8 s_json_write_value(s_json_writer* _wr, const s_json* _node) {
    if (_node == NULL) return false;
    switch (_node->type) {
        case S_JSON_NULL: return s_json_writer_append(_wr, "null", 4);
        case S_JSON_BOOL: return s_json_writer_append(_wr, _node->as.boolean ? "true" : "false", _node->as.boolean ? 4 : 5);
        case S_JSON_NUMBER: return s_json_writer_append_number(_wr, _node->as.number);
        case S_JSON_STRING: return s_json_writer_append_escaped(_wr, _node->as.string ? _node->as.string : "");
        case S_JSON_ARRAY: return s_json_write_array(_wr, _node);
        case S_JSON_OBJECT: return s_json_write_object(_wr, _node);
        default: return false;
    }
}

S_JSON_DEF char* s_json_stringify_precision(const s_json* _node, i32 _number_precision) {
    s_json_writer wr = {0};
    wr.grow = true;
    wr.number_precision = _number_precision;
    if (!s_json_write_value(&wr, _node) || wr.failed) {
        s_free(wr.data);
        return NULL;
    }
    return wr.data;
}

S_JSON_DEF char* s_json_stringify(const s_json* _node) {
    return s_json_stringify_precision(_node, -1);
}

S_JSON_DEF sz s_json_write_precision(const s_json* _node, char* _buffer, sz _capacity, i32 _number_precision) {
    if (_buffer == NULL || _capacity == 0) return 0;
    s_json_writer wr = {0};
    wr.data = _buffer;
    wr.capacity = _capacity;
    wr.grow = false;
    wr.number_precision = _number_precision;
    _buffer[0] = '\0';
    if (!s_json_write_value(&wr, _node) || wr.failed) return 0;
    return wr.size;
}

S_JSON_DEF sz s_json_write(const s_json* _node, char* _buffer, sz _capacity) {
    return s_json_write_precision(_node, _buffer, _capacity, -1);
}

typedef struct {
    const char* text;
    sz len;
    sz pos;
    sz line;
    sz column;
    sz depth;
    s_json_error error;
} s_json_parser;

S_JSON_PRIV void s_json_set_error(s_json_parser* _p, s_json_error_code _code) {
    if (_p->error.code != S_JSON_OK) return;
    _p->error.code = _code;
    _p->error.offset = _p->pos;
    _p->error.line = _p->line;
    _p->error.column = _p->column;
}

S_JSON_PRIV char s_json_peek(s_json_parser* _p) {
    if (_p->pos >= _p->len) return '\0';
    return _p->text[_p->pos];
}

S_JSON_PRIV char s_json_next(s_json_parser* _p) {
    const char c = s_json_peek(_p);
    if (_p->pos < _p->len) {
        _p->pos++;
        if (c == '\n') {
            _p->line++;
            _p->column = 1;
        } else {
            _p->column++;
        }
    }
    return c;
}

S_JSON_PRIV void s_json_skip_ws(s_json_parser* _p) {
    while (_p->pos < _p->len) {
        char c = s_json_peek(_p);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') s_json_next(_p);
        else break;
    }
}

S_JSON_PRIV b8 s_json_hex_val(char c, u8* out) {
    if (c >= '0' && c <= '9') { *out = (u8)(c - '0'); return true; }
    if (c >= 'a' && c <= 'f') { *out = (u8)(10 + (c - 'a')); return true; }
    if (c >= 'A' && c <= 'F') { *out = (u8)(10 + (c - 'A')); return true; }
    return false;
}

S_JSON_PRIV b8 s_json_parse_hex4(s_json_parser* _p, u32* _out) {
    u8 h1, h2, h3, h4;
    if (_p == NULL || _out == NULL) return false;
    if (!s_json_hex_val(s_json_next(_p), &h1) ||
        !s_json_hex_val(s_json_next(_p), &h2) ||
        !s_json_hex_val(s_json_next(_p), &h3) ||
        !s_json_hex_val(s_json_next(_p), &h4)) {
        return false;
    }
    *_out = (u32)((h1 << 12) | (h2 << 8) | (h3 << 4) | h4);
    return true;
}

S_JSON_PRIV b8 s_json_append_utf8(char** buf, sz* size, sz* cap, u32 cp) {
    unsigned char tmp[4];
    sz len = 0;
    if (buf == NULL || size == NULL || cap == NULL) return false;
    if (cp > 0x10FFFF || (cp >= 0xD800 && cp <= 0xDFFF)) return false;
    if (cp <= 0x7F) {
        tmp[0] = (unsigned char)cp;
        len = 1;
    } else if (cp <= 0x7FF) {
        tmp[0] = 0xC0 | (cp >> 6);
        tmp[1] = 0x80 | (cp & 0x3F);
        len = 2;
    } else if (cp <= 0xFFFF) {
        tmp[0] = 0xE0 | (cp >> 12);
        tmp[1] = 0x80 | ((cp >> 6) & 0x3F);
        tmp[2] = 0x80 | (cp & 0x3F);
        len = 3;
    } else {
        tmp[0] = 0xF0 | (cp >> 18);
        tmp[1] = 0x80 | ((cp >> 12) & 0x3F);
        tmp[2] = 0x80 | ((cp >> 6) & 0x3F);
        tmp[3] = 0x80 | (cp & 0x3F);
        len = 4;
    }
    if (*size > SIZE_MAX - len || *size + len > SIZE_MAX - 1) return false;
    if (!s_json_buffer_reserve(buf, cap, *size + len + 1)) return false;
    memcpy(*buf + *size, tmp, len);
    *size += len;
    (*buf)[*size] = '\0';
    return true;
}

S_JSON_PRIV b8 s_json_append_bytes(char** buf, sz* size, sz* cap, const char* src, sz len) {
    if (buf == NULL || size == NULL || cap == NULL || (len > 0 && src == NULL)) return false;
    if (*size > SIZE_MAX - len || *size + len > SIZE_MAX - 1) return false;
    if (!s_json_buffer_reserve(buf, cap, *size + len + 1)) return false;
    if (len > 0) memcpy(*buf + *size, src, len);
    *size += len;
    (*buf)[*size] = '\0';
    return true;
}

S_JSON_PRIV char* s_json_parse_string(s_json_parser* _p) {
    if (s_json_next(_p) != '"') return NULL;
    char* buf = NULL;
    sz size = 0;
    sz cap = 0;
    while (_p->pos < _p->len) {
        char c = s_json_next(_p);
        if (c == '"') {
            if (buf != NULL) return buf;
            buf = s_json_strdup("");
            if (buf == NULL) s_json_set_error(_p, S_JSON_ERR_OOM);
            return buf;
        }
        if ((unsigned char)c < 0x20) { s_json_set_error(_p, S_JSON_ERR_INVALID_STRING); s_free(buf); return NULL; }
        if (c == '\\') {
            char esc = s_json_next(_p);
            switch (esc) {
                case '"': if (!s_json_append_utf8(&buf, &size, &cap, '"')) goto oom; break;
                case '\\': if (!s_json_append_utf8(&buf, &size, &cap, '\\')) goto oom; break;
                case '/': if (!s_json_append_utf8(&buf, &size, &cap, '/')) goto oom; break;
                case 'b': if (!s_json_append_utf8(&buf, &size, &cap, '\b')) goto oom; break;
                case 'f': if (!s_json_append_utf8(&buf, &size, &cap, '\f')) goto oom; break;
                case 'n': if (!s_json_append_utf8(&buf, &size, &cap, '\n')) goto oom; break;
                case 'r': if (!s_json_append_utf8(&buf, &size, &cap, '\r')) goto oom; break;
                case 't': if (!s_json_append_utf8(&buf, &size, &cap, '\t')) goto oom; break;
                case 'u': {
                    u32 code = 0;
                    if (!s_json_parse_hex4(_p, &code)) goto invalid;
                    if (code >= 0xD800 && code <= 0xDBFF) {
                        if (s_json_next(_p) != '\\' || s_json_next(_p) != 'u') goto invalid;
                        u32 low = 0;
                        if (!s_json_parse_hex4(_p, &low)) goto invalid;
                        if (low < 0xDC00 || low > 0xDFFF) goto invalid;
                        code = 0x10000 + (((code - 0xD800) << 10) | (low - 0xDC00));
                    } else if (code >= 0xDC00 && code <= 0xDFFF) {
                        goto invalid;
                    }
                    if (!s_json_append_utf8(&buf, &size, &cap, code)) goto oom;
                    break;
                }
                default:
                    goto invalid;
            }
        } else {
            const char* raw = _p->text + _p->pos - 1;
            sz byte_count = 0;
            if (!s_json_cstring_utf8_len((const unsigned char*)raw, &byte_count)) goto invalid;
            for (sz i = 1; i < byte_count; ++i) s_json_next(_p);
            if (!s_json_append_bytes(&buf, &size, &cap, raw, byte_count)) goto oom;
        }
    }
    s_json_set_error(_p, S_JSON_ERR_EOF);
    s_free(buf);
    return NULL;
invalid:
    s_json_set_error(_p, S_JSON_ERR_INVALID_STRING);
    s_free(buf);
    return NULL;
oom:
    s_json_set_error(_p, S_JSON_ERR_OOM);
    s_free(buf);
    return NULL;
}

S_JSON_PRIV s_json* s_json_parse_value(s_json_parser* _p);

S_JSON_PRIV s_json* s_json_parse_array(s_json_parser* _p) {
    if (_p->depth >= S_JSON_MAX_DEPTH) {
        s_json_set_error(_p, S_JSON_ERR_DEPTH);
        return NULL;
    }
    _p->depth++;
    if (s_json_next(_p) != '[') return NULL;
    s_json* arr = s_json_array_empty(NULL);
    if (arr == NULL) { _p->depth--; s_json_set_error(_p, S_JSON_ERR_OOM); return NULL; }
    s_json_skip_ws(_p);
    if (s_json_peek(_p) == ']') { s_json_next(_p); _p->depth--; return arr; }
    while (_p->pos < _p->len) {
        s_json* val = s_json_parse_value(_p);
        if (val == NULL) { s_json_free(arr); _p->depth--; return NULL; }
        if (!s_json_add(arr, val)) { s_json_set_error(_p, S_JSON_ERR_OOM); s_json_free(val); s_json_free(arr); _p->depth--; return NULL; }
        s_json_skip_ws(_p);
        char c = s_json_next(_p);
        if (c == ']') { _p->depth--; return arr; }
        if (c != ',') { s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); break; }
        s_json_skip_ws(_p);
    }
    s_json_free(arr);
    _p->depth--;
    return NULL;
}

S_JSON_PRIV s_json* s_json_parse_object(s_json_parser* _p) {
    if (_p->depth >= S_JSON_MAX_DEPTH) {
        s_json_set_error(_p, S_JSON_ERR_DEPTH);
        return NULL;
    }
    _p->depth++;
    if (s_json_next(_p) != '{') return NULL;
    s_json* obj = s_json_object_empty(NULL);
    if (obj == NULL) { _p->depth--; s_json_set_error(_p, S_JSON_ERR_OOM); return NULL; }
    s_json_skip_ws(_p);
    if (s_json_peek(_p) == '}') { s_json_next(_p); _p->depth--; return obj; }
    while (_p->pos < _p->len) {
        if (s_json_peek(_p) != '"') { s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); break; }
        char* key = s_json_parse_string(_p);
        if (key == NULL) { s_json_free(obj); _p->depth--; return NULL; }
        s_json_skip_ws(_p);
        if (s_json_next(_p) != ':') { s_free(key); s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); break; }
        s_json_skip_ws(_p);
        s_json* val = s_json_parse_value(_p);
        if (val == NULL) { s_free(key); s_json_free(obj); _p->depth--; return NULL; }
        s_json_set_name_take(val, key);
        if (!s_json_add(obj, val)) { s_json_set_error(_p, S_JSON_ERR_OOM); s_json_free(val); s_json_free(obj); _p->depth--; return NULL; }
        s_json_skip_ws(_p);
        char c = s_json_next(_p);
        if (c == '}') { _p->depth--; return obj; }
        if (c != ',') { s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); break; }
        s_json_skip_ws(_p);
    }
    s_json_free(obj);
    _p->depth--;
    return NULL;
}

S_JSON_PRIV s_json* s_json_parse_number(s_json_parser* _p) {
    const sz start_pos = _p->pos;
    char* end = NULL;
    f64 num = 0.0;

    if (s_json_peek(_p) == '-') s_json_next(_p);

    if (s_json_peek(_p) == '0') {
        s_json_next(_p);
        if (s_json_peek(_p) >= '0' && s_json_peek(_p) <= '9') {
            s_json_set_error(_p, S_JSON_ERR_INVALID_NUMBER);
            return NULL;
        }
    } else if (s_json_peek(_p) >= '1' && s_json_peek(_p) <= '9') {
        while (s_json_peek(_p) >= '0' && s_json_peek(_p) <= '9') s_json_next(_p);
    } else {
        s_json_set_error(_p, S_JSON_ERR_INVALID_NUMBER);
        return NULL;
    }

    if (s_json_peek(_p) == '.') {
        s_json_next(_p);
        if (!(s_json_peek(_p) >= '0' && s_json_peek(_p) <= '9')) {
            s_json_set_error(_p, S_JSON_ERR_INVALID_NUMBER);
            return NULL;
        }
        while (s_json_peek(_p) >= '0' && s_json_peek(_p) <= '9') s_json_next(_p);
    }

    if (s_json_peek(_p) == 'e' || s_json_peek(_p) == 'E') {
        s_json_next(_p);
        if (s_json_peek(_p) == '+' || s_json_peek(_p) == '-') s_json_next(_p);
        if (!(s_json_peek(_p) >= '0' && s_json_peek(_p) <= '9')) {
            s_json_set_error(_p, S_JSON_ERR_INVALID_NUMBER);
            return NULL;
        }
        while (s_json_peek(_p) >= '0' && s_json_peek(_p) <= '9') s_json_next(_p);
    }

    errno = 0;
    num = strtod(_p->text + start_pos, &end);
    if (end != _p->text + _p->pos || errno == ERANGE || !isfinite(num)) {
        s_json_set_error(_p, S_JSON_ERR_INVALID_NUMBER);
        return NULL;
    }
    s_json* node = s_json_num(NULL, num);
    if (node == NULL) {
        s_json_set_error(_p, S_JSON_ERR_OOM);
        return NULL;
    }
    return node;
}

S_JSON_PRIV b8 s_json_consume_literal(s_json_parser* _p, const char* _literal, sz _len) {
    if (_p == NULL || _literal == NULL || _len > _p->len - _p->pos) return false;
    if (strncmp(_p->text + _p->pos, _literal, _len) != 0) return false;
    for (sz i = 0; i < _len; ++i) s_json_next(_p);
    return true;
}

S_JSON_PRIV s_json* s_json_node_or_oom(s_json_parser* _p, s_json* _node) {
    if (_node == NULL) s_json_set_error(_p, S_JSON_ERR_OOM);
    return _node;
}

S_JSON_PRIV s_json* s_json_parse_value(s_json_parser* _p) {
    s_json_skip_ws(_p);
    char c = s_json_peek(_p);
    if (c == '\0') { s_json_set_error(_p, S_JSON_ERR_EOF); return NULL; }
    if (c == '{') return s_json_parse_object(_p);
    if (c == '[') return s_json_parse_array(_p);
    if (c == '"') {
        char* str = s_json_parse_string(_p);
        if (str == NULL) return NULL;
        s_json* node = s_json_str_take(NULL, str);
        if (node == NULL) s_json_set_error(_p, S_JSON_ERR_OOM);
        return node;
    }
    if (c == 't') {
        if (s_json_consume_literal(_p, "true", 4)) return s_json_node_or_oom(_p, s_json_bool(NULL, true));
        s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); return NULL;
    }
    if (c == 'f') {
        if (s_json_consume_literal(_p, "false", 5)) return s_json_node_or_oom(_p, s_json_bool(NULL, false));
        s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); return NULL;
    }
    if (c == 'n') {
        if (s_json_consume_literal(_p, "null", 4)) return s_json_node_or_oom(_p, s_json_null(NULL));
        s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); return NULL;
    }
    if (c == '-' || (c >= '0' && c <= '9')) return s_json_parse_number(_p);
    s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR);
    return NULL;
}

S_JSON_DEF s_json* s_json_parse_with_error(const char* _text, s_json_error* _out_err) {
    s_json_parser p = {0};
    p.text = _text ? _text : "";
    p.len = strlen(p.text);
    p.pos = 0;
    p.line = 1;
    p.column = 1;
    p.error.code = S_JSON_OK;
    s_json* root = s_json_parse_value(&p);
    if (root == NULL) {
        if (_out_err != NULL) *_out_err = p.error;
        return NULL;
    }
    s_json_skip_ws(&p);
    if (p.pos != p.len) {
        s_json_set_error(&p, S_JSON_ERR_TRAILING_CHAR);
        s_json_free(root);
        if (_out_err != NULL) *_out_err = p.error;
        return NULL;
    }
    if (_out_err != NULL) *_out_err = p.error;
    return root;
}

S_JSON_DEF s_json* s_json_parse(const char* _text) {
    return s_json_parse_with_error(_text, NULL);
}

#undef S_JSON_DEF
#undef S_JSON_PRIV
#endif

#endif // S_JSON_H
