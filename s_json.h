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
    S_JSON_ERR_TRAILING_CHAR
} s_json_error_code;

typedef struct {
    s_json_error_code code;
    sz offset;
    sz line;
    sz column;
} s_json_error;

static inline char* s_json_strdup(const char* _str) {
    if (_str == NULL) return NULL;
    const sz len = strlen(_str);
    char* out = (char*)malloc(len + 1);
    if (out == NULL) return NULL;
    memcpy(out, _str, len);
    out[len] = '\0';
    return out;
}

static inline s_json* s_json_new(s_json_type _type, const char* _name) {
    s_json* node = (s_json*)malloc(sizeof(s_json));
    if (node == NULL) return NULL;
    node->type = _type;
    node->name = s_json_strdup(_name);
    if (_name != NULL && node->name == NULL) {
        free(node);
        return NULL;
    }
    if (_type == S_JSON_ARRAY || _type == S_JSON_OBJECT) {
        node->as.children.items = NULL;
        node->as.children.count = 0;
        node->as.children.capacity = 0;
    } else if (_type == S_JSON_STRING) {
        node->as.string = NULL;
    } else if (_type == S_JSON_BOOL) {
        node->as.boolean = false;
    } else if (_type == S_JSON_NUMBER) {
        node->as.number = 0.0;
    }
    return node;
}

static inline s_json* s_json_new_take(s_json_type _type, char* _name) {
    s_json* node = (s_json*)malloc(sizeof(s_json));
    if (node == NULL) return NULL;
    node->type = _type;
    node->name = _name;
    if (_type == S_JSON_ARRAY || _type == S_JSON_OBJECT) {
        node->as.children.items = NULL;
        node->as.children.count = 0;
        node->as.children.capacity = 0;
    } else if (_type == S_JSON_STRING) {
        node->as.string = NULL;
    } else if (_type == S_JSON_BOOL) {
        node->as.boolean = false;
    } else if (_type == S_JSON_NUMBER) {
        node->as.number = 0.0;
    }
    return node;
}

static inline void s_json_set_name_take(s_json* _node, char* _name) {
    s_assertf(_node != NULL, "s_json_set_name_take :: Node is null\n");
    if (_node->name != NULL) free(_node->name);
    _node->name = _name;
}

static inline void s_json_set_name(s_json* _node, const char* _name) {
    s_assertf(_node != NULL, "s_json_set_name :: Node is null\n");
    if (_node->name != NULL) {
        free(_node->name);
        _node->name = NULL;
    }
    if (_name != NULL) _node->name = s_json_strdup(_name);
}

static inline s_json* s_json_null(const char* _name) {
    return s_json_new(S_JSON_NULL, _name);
}

static inline s_json* s_json_bool(const char* _name, b8 _value) {
    s_json* node = s_json_new(S_JSON_BOOL, _name);
    if (node == NULL) return NULL;
    node->as.boolean = _value;
    return node;
}

static inline s_json* s_json_int(const char* _name, i64 _value) {
    s_json* node = s_json_new(S_JSON_NUMBER, _name);
    if (node == NULL) return NULL;
    node->as.number = (f64)_value;
    return node;
}

static inline s_json* s_json_num(const char* _name, f64 _value) {
    s_json* node = s_json_new(S_JSON_NUMBER, _name);
    if (node == NULL) return NULL;
    node->as.number = _value;
    return node;
}

static inline s_json* s_json_str_take(const char* _name, char* _value) {
    s_json* node = s_json_new(S_JSON_STRING, _name);
    if (node == NULL) {
        free(_value);
        return NULL;
    }
    node->as.string = _value;
    return node;
}

static inline s_json* s_json_str(const char* _name, const char* _value) {
    s_json* node = s_json_new(S_JSON_STRING, _name);
    if (node == NULL) return NULL;
    node->as.string = s_json_strdup(_value ? _value : "");
    if (node->as.string == NULL) {
        free(node->name);
        free(node);
        return NULL;
    }
    return node;
}

static inline b8 s_json_children_reserve(s_json_children* _children, sz _cap) {
    if (_cap <= _children->capacity) return true;
    sz new_cap = _children->capacity == 0 ? 4 : _children->capacity;
    while (new_cap < _cap) new_cap *= 2;
    s_json** next = (s_json**)realloc(_children->items, new_cap * sizeof(s_json*));
    if (next == NULL) return false;
    _children->items = next;
    _children->capacity = new_cap;
    return true;
}

static inline b8 s_json_add(s_json* _parent, s_json* _child) {
    s_assertf(_parent != NULL, "s_json_add :: Parent is null\n");
    s_assertf(_child != NULL, "s_json_add :: Child is null\n");
    if (!(_parent->type == S_JSON_ARRAY || _parent->type == S_JSON_OBJECT)) return false;
    if (!s_json_children_reserve(&_parent->as.children, _parent->as.children.count + 1)) return false;
    _parent->as.children.items[_parent->as.children.count++] = _child;
    return true;
}

static inline s_json* s_json_array_n(const char* _name, sz _count, s_json** _items) {
    s_json* node = s_json_new(S_JSON_ARRAY, _name);
    if (node == NULL) return NULL;
    if (_count > 0) {
        node->as.children.items = (s_json**)malloc(sizeof(s_json*) * _count);
        if (node->as.children.items == NULL) {
            free(node->name);
            free(node);
            return NULL;
        }
        memcpy(node->as.children.items, _items, sizeof(s_json*) * _count);
        node->as.children.count = _count;
        node->as.children.capacity = _count;
    }
    return node;
}

static inline s_json* s_json_object_n(const char* _name, sz _count, s_json** _items) {
    s_json* node = s_json_new(S_JSON_OBJECT, _name);
    if (node == NULL) return NULL;
    if (_count > 0) {
        node->as.children.items = (s_json**)malloc(sizeof(s_json*) * _count);
        if (node->as.children.items == NULL) {
            free(node->name);
            free(node);
            return NULL;
        }
        memcpy(node->as.children.items, _items, sizeof(s_json*) * _count);
        node->as.children.count = _count;
        node->as.children.capacity = _count;
    }
    return node;
}

#define s_json_array_empty(_name) s_json_array_n((_name), 0, NULL)
#define s_json_object_empty(_name) s_json_object_n((_name), 0, NULL)

#define s_json_array(_name, ...) \
    s_json_array_n((_name), (sz)(sizeof((s_json*[]){__VA_ARGS__}) / sizeof(s_json*)), (s_json*[]){__VA_ARGS__})

#define s_json_object(_name, ...) \
    s_json_object_n((_name), (sz)(sizeof((s_json*[]){__VA_ARGS__}) / sizeof(s_json*)), (s_json*[]){__VA_ARGS__})

static inline s_json* s_json_get(const s_json* _obj, const char* _key) {
    if (_obj == NULL || _obj->type != S_JSON_OBJECT || _key == NULL) return NULL;
    for (sz i = 0; i < _obj->as.children.count; i++) {
        s_json* child = _obj->as.children.items[i];
        if (child != NULL && child->name != NULL && strcmp(child->name, _key) == 0) return child;
    }
    return NULL;
}

static inline s_json* s_json_at(const s_json* _arr, sz _index) {
    if (_arr == NULL || _arr->type != S_JSON_ARRAY) return NULL;
    if (_index >= _arr->as.children.count) return NULL;
    return _arr->as.children.items[_index];
}

static inline void s_json_free(s_json* _node) {
    if (_node == NULL) return;
    if (_node->type == S_JSON_STRING && _node->as.string != NULL) {
        free(_node->as.string);
    }
    if (_node->type == S_JSON_ARRAY || _node->type == S_JSON_OBJECT) {
        for (sz i = 0; i < _node->as.children.count; i++) {
            s_json_free(_node->as.children.items[i]);
        }
        free(_node->as.children.items);
    }
    free(_node->name);
    free(_node);
}

typedef struct {
    char* data;
    sz size;
    sz capacity;
    b8 grow;
    b8 failed;
} s_json_writer;

static inline b8 s_json_writer_reserve(s_json_writer* _wr, sz _need) {
    if (_wr->failed) return false;
    if (_need <= _wr->capacity) return true;
    if (!_wr->grow) {
        _wr->failed = true;
        return false;
    }
    sz new_cap = _wr->capacity == 0 ? 128 : _wr->capacity;
    while (new_cap < _need) new_cap *= 2;
    char* next = (char*)realloc(_wr->data, new_cap);
    if (next == NULL) {
        _wr->failed = true;
        return false;
    }
    _wr->data = next;
    _wr->capacity = new_cap;
    return true;
}

static inline b8 s_json_writer_append(s_json_writer* _wr, const char* _data, sz _len) {
    if (!s_json_writer_reserve(_wr, _wr->size + _len + 1)) return false;
    memcpy(_wr->data + _wr->size, _data, _len);
    _wr->size += _len;
    _wr->data[_wr->size] = '\0';
    return true;
}

static inline b8 s_json_writer_append_char(s_json_writer* _wr, char _c) {
    return s_json_writer_append(_wr, &_c, 1);
}

static inline b8 s_json_writer_append_escaped(s_json_writer* _wr, const char* _str) {
    if (!s_json_writer_append_char(_wr, '"')) return false;
    for (const unsigned char* p = (const unsigned char*)_str; *p; p++) {
        const unsigned char c = *p;
        switch (c) {
            case '"': if (!s_json_writer_append(_wr, "\\\"", 2)) return false; break;
            case '\\': if (!s_json_writer_append(_wr, "\\\\", 2)) return false; break;
            case '\b': if (!s_json_writer_append(_wr, "\\b", 2)) return false; break;
            case '\f': if (!s_json_writer_append(_wr, "\\f", 2)) return false; break;
            case '\n': if (!s_json_writer_append(_wr, "\\n", 2)) return false; break;
            case '\r': if (!s_json_writer_append(_wr, "\\r", 2)) return false; break;
            case '\t': if (!s_json_writer_append(_wr, "\\t", 2)) return false; break;
            default:
                if (c < 0x20) {
                    char buf[7];
                    snprintf(buf, sizeof(buf), "\\u%04x", c);
                    if (!s_json_writer_append(_wr, buf, 6)) return false;
                } else {
                    if (!s_json_writer_append_char(_wr, (char)c)) return false;
                }
                break;
        }
    }
    return s_json_writer_append_char(_wr, '"');
}

static inline b8 s_json_write_value(s_json_writer* _wr, const s_json* _node);

static inline b8 s_json_write_array(s_json_writer* _wr, const s_json* _node) {
    if (!s_json_writer_append_char(_wr, '[')) return false;
    for (sz i = 0; i < _node->as.children.count; i++) {
        if (i > 0 && !s_json_writer_append_char(_wr, ',')) return false;
        if (!s_json_write_value(_wr, _node->as.children.items[i])) return false;
    }
    return s_json_writer_append_char(_wr, ']');
}

static inline b8 s_json_write_object(s_json_writer* _wr, const s_json* _node) {
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

static inline b8 s_json_write_value(s_json_writer* _wr, const s_json* _node) {
    if (_node == NULL) return false;
    switch (_node->type) {
        case S_JSON_NULL: return s_json_writer_append(_wr, "null", 4);
        case S_JSON_BOOL: return s_json_writer_append(_wr, _node->as.boolean ? "true" : "false", _node->as.boolean ? 4 : 5);
        case S_JSON_NUMBER: {
            char buf[64];
            const int len = snprintf(buf, sizeof(buf), "%.17g", _node->as.number);
            if (len <= 0) return false;
            return s_json_writer_append(_wr, buf, (sz)len);
        }
        case S_JSON_STRING: return s_json_writer_append_escaped(_wr, _node->as.string ? _node->as.string : "");
        case S_JSON_ARRAY: return s_json_write_array(_wr, _node);
        case S_JSON_OBJECT: return s_json_write_object(_wr, _node);
        default: return false;
    }
}

static inline char* s_json_stringify(const s_json* _node) {
    s_json_writer wr = {0};
    wr.grow = true;
    if (!s_json_write_value(&wr, _node) || wr.failed) {
        free(wr.data);
        return NULL;
    }
    return wr.data;
}

static inline sz s_json_write(const s_json* _node, char* _buffer, sz _capacity) {
    s_json_writer wr = {0};
    wr.data = _buffer;
    wr.capacity = _capacity;
    wr.grow = false;
    if (_buffer != NULL && _capacity > 0) _buffer[0] = '\0';
    if (!s_json_write_value(&wr, _node) || wr.failed) return 0;
    return wr.size;
}

typedef struct {
    const char* text;
    sz len;
    sz pos;
    sz line;
    sz column;
    s_json_error error;
} s_json_parser;

static inline void s_json_set_error(s_json_parser* _p, s_json_error_code _code) {
    if (_p->error.code != S_JSON_OK) return;
    _p->error.code = _code;
    _p->error.offset = _p->pos;
    _p->error.line = _p->line;
    _p->error.column = _p->column;
}

static inline char s_json_peek(s_json_parser* _p) {
    if (_p->pos >= _p->len) return '\0';
    return _p->text[_p->pos];
}

static inline char s_json_next(s_json_parser* _p) {
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

static inline void s_json_skip_ws(s_json_parser* _p) {
    while (_p->pos < _p->len) {
        char c = s_json_peek(_p);
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') s_json_next(_p);
        else break;
    }
}

static inline b8 s_json_hex_val(char c, u8* out) {
    if (c >= '0' && c <= '9') { *out = (u8)(c - '0'); return true; }
    if (c >= 'a' && c <= 'f') { *out = (u8)(10 + (c - 'a')); return true; }
    if (c >= 'A' && c <= 'F') { *out = (u8)(10 + (c - 'A')); return true; }
    return false;
}

static inline b8 s_json_append_utf8(char** buf, sz* size, sz* cap, u32 cp) {
    unsigned char tmp[4];
    sz len = 0;
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
    if (*size + len + 1 > *cap) {
        sz new_cap = *cap == 0 ? 16 : *cap;
        while (new_cap < *size + len + 1) new_cap *= 2;
        char* next = (char*)realloc(*buf, new_cap);
        if (next == NULL) return false;
        *buf = next;
        *cap = new_cap;
    }
    memcpy(*buf + *size, tmp, len);
    *size += len;
    (*buf)[*size] = '\0';
    return true;
}

static inline char* s_json_parse_string(s_json_parser* _p) {
    if (s_json_next(_p) != '"') return NULL;
    char* buf = NULL;
    sz size = 0;
    sz cap = 0;
    while (_p->pos < _p->len) {
        char c = s_json_next(_p);
        if (c == '"') return buf ? buf : s_json_strdup("");
        if ((unsigned char)c < 0x20) { s_json_set_error(_p, S_JSON_ERR_INVALID_STRING); free(buf); return NULL; }
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
                    u8 h1, h2, h3, h4;
                    if (!s_json_hex_val(s_json_next(_p), &h1) ||
                        !s_json_hex_val(s_json_next(_p), &h2) ||
                        !s_json_hex_val(s_json_next(_p), &h3) ||
                        !s_json_hex_val(s_json_next(_p), &h4)) {
                        s_json_set_error(_p, S_JSON_ERR_INVALID_STRING);
                        free(buf);
                        return NULL;
                    }
                    u32 code = (u32)((h1 << 12) | (h2 << 8) | (h3 << 4) | h4);
                    if (code >= 0xD800 && code <= 0xDBFF) {
                        if (s_json_next(_p) != '\\' || s_json_next(_p) != 'u') {
                            s_json_set_error(_p, S_JSON_ERR_INVALID_STRING);
                            free(buf);
                            return NULL;
                        }
                        if (!s_json_hex_val(s_json_next(_p), &h1) ||
                            !s_json_hex_val(s_json_next(_p), &h2) ||
                            !s_json_hex_val(s_json_next(_p), &h3) ||
                            !s_json_hex_val(s_json_next(_p), &h4)) {
                            s_json_set_error(_p, S_JSON_ERR_INVALID_STRING);
                            free(buf);
                            return NULL;
                        }
                        u32 low = (u32)((h1 << 12) | (h2 << 8) | (h3 << 4) | h4);
                        if (low < 0xDC00 || low > 0xDFFF) {
                            s_json_set_error(_p, S_JSON_ERR_INVALID_STRING);
                            free(buf);
                            return NULL;
                        }
                        code = 0x10000 + (((code - 0xD800) << 10) | (low - 0xDC00));
                    }
                    if (!s_json_append_utf8(&buf, &size, &cap, code)) goto oom;
                    break;
                }
                default:
                    s_json_set_error(_p, S_JSON_ERR_INVALID_STRING);
                    free(buf);
                    return NULL;
            }
        } else {
            if (!s_json_append_utf8(&buf, &size, &cap, (unsigned char)c)) goto oom;
        }
    }
    s_json_set_error(_p, S_JSON_ERR_EOF);
    free(buf);
    return NULL;
oom:
    s_json_set_error(_p, S_JSON_ERR_OOM);
    free(buf);
    return NULL;
}

static inline s_json* s_json_parse_value(s_json_parser* _p);

static inline s_json* s_json_parse_array(s_json_parser* _p) {
    if (s_json_next(_p) != '[') return NULL;
    s_json* arr = s_json_array_empty(NULL);
    if (arr == NULL) { s_json_set_error(_p, S_JSON_ERR_OOM); return NULL; }
    s_json_skip_ws(_p);
    if (s_json_peek(_p) == ']') { s_json_next(_p); return arr; }
    while (_p->pos < _p->len) {
        s_json* val = s_json_parse_value(_p);
        if (val == NULL) { s_json_free(arr); return NULL; }
        if (!s_json_add(arr, val)) { s_json_set_error(_p, S_JSON_ERR_OOM); s_json_free(val); s_json_free(arr); return NULL; }
        s_json_skip_ws(_p);
        char c = s_json_next(_p);
        if (c == ']') return arr;
        if (c != ',') { s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); break; }
        s_json_skip_ws(_p);
    }
    s_json_free(arr);
    return NULL;
}

static inline s_json* s_json_parse_object(s_json_parser* _p) {
    if (s_json_next(_p) != '{') return NULL;
    s_json* obj = s_json_object_empty(NULL);
    if (obj == NULL) { s_json_set_error(_p, S_JSON_ERR_OOM); return NULL; }
    s_json_skip_ws(_p);
    if (s_json_peek(_p) == '}') { s_json_next(_p); return obj; }
    while (_p->pos < _p->len) {
        if (s_json_peek(_p) != '"') { s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); break; }
        char* key = s_json_parse_string(_p);
        if (key == NULL) { s_json_free(obj); return NULL; }
        s_json_skip_ws(_p);
        if (s_json_next(_p) != ':') { free(key); s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); break; }
        s_json_skip_ws(_p);
        s_json* val = s_json_parse_value(_p);
        if (val == NULL) { free(key); s_json_free(obj); return NULL; }
        s_json_set_name_take(val, key);
        if (!s_json_add(obj, val)) { s_json_set_error(_p, S_JSON_ERR_OOM); s_json_free(val); s_json_free(obj); return NULL; }
        s_json_skip_ws(_p);
        char c = s_json_next(_p);
        if (c == '}') return obj;
        if (c != ',') { s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); break; }
        s_json_skip_ws(_p);
    }
    s_json_free(obj);
    return NULL;
}

static inline s_json* s_json_parse_number(s_json_parser* _p) {
    const char* start = _p->text + _p->pos;
    char* end = NULL;
    f64 num = strtod(start, &end);
    if (end == start) { s_json_set_error(_p, S_JSON_ERR_INVALID_NUMBER); return NULL; }
    _p->pos = (sz)(end - _p->text);
    return s_json_num(NULL, num);
}

static inline s_json* s_json_parse_value(s_json_parser* _p) {
    s_json_skip_ws(_p);
    char c = s_json_peek(_p);
    if (c == '\0') { s_json_set_error(_p, S_JSON_ERR_EOF); return NULL; }
    if (c == '{') return s_json_parse_object(_p);
    if (c == '[') return s_json_parse_array(_p);
    if (c == '"') {
        char* str = s_json_parse_string(_p);
        if (str == NULL) return NULL;
        return s_json_str_take(NULL, str);
    }
    if (c == 't') {
        if (_p->pos + 4 <= _p->len && strncmp(_p->text + _p->pos, "true", 4) == 0) { _p->pos += 4; return s_json_bool(NULL, true); }
        s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); return NULL;
    }
    if (c == 'f') {
        if (_p->pos + 5 <= _p->len && strncmp(_p->text + _p->pos, "false", 5) == 0) { _p->pos += 5; return s_json_bool(NULL, false); }
        s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); return NULL;
    }
    if (c == 'n') {
        if (_p->pos + 4 <= _p->len && strncmp(_p->text + _p->pos, "null", 4) == 0) { _p->pos += 4; return s_json_null(NULL); }
        s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR); return NULL;
    }
    if (c == '-' || (c >= '0' && c <= '9')) return s_json_parse_number(_p);
    s_json_set_error(_p, S_JSON_ERR_UNEXPECTED_CHAR);
    return NULL;
}

static inline s_json* s_json_parse_with_error(const char* _text, s_json_error* _out_err) {
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

static inline s_json* s_json_parse(const char* _text) {
    return s_json_parse_with_error(_text, NULL);
}

#endif // S_JSON_H
