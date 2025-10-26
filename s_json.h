// Syphax - Ougi-Washi

#ifndef S_JSON_H
#define S_JSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdbool.h>

typedef enum {
    S_JSON_TYPE_NULL,
    S_JSON_TYPE_BOOL,
    S_JSON_TYPE_NUMBER,
    S_JSON_TYPE_STRING,
    S_JSON_TYPE_ARRAY,
    S_JSON_TYPE_OBJECT,
} s_json_type;

typedef struct s_json {
    s_json_type type;
    char *key;
    union {
        bool b;
        double d;
        int i;
        char *s;
        struct s_json *object;
        struct s_json *array;
    } value;
    struct s_json *next;  // For linking array/object elements
    int count;            // Number of elements in array/object
} s_json;

typedef struct {
    const char *json;
    size_t pos;
    size_t len;
} parser_state;

static s_json* parse_value(parser_state *state);
static void s_json_free(s_json *j);

static void skip_whitespace(parser_state *state) {
    while (state->pos < state->len && isspace(state->json[state->pos])) {
        state->pos++;
    }
}

static s_json* create_node(s_json_type type) {
    s_json *node = (s_json*)calloc(1, sizeof(s_json));
    if (!node) return NULL;
    node->type = type;
    node->key = NULL;
    node->next = NULL;
    node->count = 0;
    return node;
}

static char* parse_string(parser_state *state) {
    if (state->json[state->pos] != '"') return NULL;
    state->pos++; // Skip opening quote
    
    size_t start = state->pos;
    size_t len = 0;
    
    // Calculate string length and validate
    while (state->pos < state->len && state->json[state->pos] != '"') {
        if (state->json[state->pos] == '\\') {
            state->pos++;
            if (state->pos >= state->len) return NULL;
        }
        state->pos++;
        len++;
    }
    
    if (state->pos >= state->len) return NULL;
    
    // Allocate and copy string
    char *str = (char*)malloc(len + 1);
    if (!str) return NULL;
    
    size_t i = 0;
    state->pos = start;
    
    while (state->json[state->pos] != '"') {
        if (state->json[state->pos] == '\\') {
            state->pos++;
            switch (state->json[state->pos]) {
                case 'n': str[i++] = '\n'; break;
                case 't': str[i++] = '\t'; break;
                case 'r': str[i++] = '\r'; break;
                case 'b': str[i++] = '\b'; break;
                case 'f': str[i++] = '\f'; break;
                case '"': str[i++] = '"'; break;
                case '\\': str[i++] = '\\'; break;
                case '/': str[i++] = '/'; break;
                default: str[i++] = state->json[state->pos];
            }
        } else {
            str[i++] = state->json[state->pos];
        }
        state->pos++;
    }
    
    str[i] = '\0';
    state->pos++; // Skip closing quote
    return str;
}

static s_json* parse_number(parser_state *state) {
    size_t start = state->pos;
    bool is_float = false;
    
    if (state->json[state->pos] == '-') state->pos++;
    
    while (state->pos < state->len && isdigit(state->json[state->pos])) {
        state->pos++;
    }
    
    if (state->pos < state->len && state->json[state->pos] == '.') {
        is_float = true;
        state->pos++;
        while (state->pos < state->len && isdigit(state->json[state->pos])) {
            state->pos++;
        }
    }
    
    if (state->pos < state->len && (state->json[state->pos] == 'e' || state->json[state->pos] == 'E')) {
        is_float = true;
        state->pos++;
        if (state->json[state->pos] == '+' || state->json[state->pos] == '-') state->pos++;
        while (state->pos < state->len && isdigit(state->json[state->pos])) {
            state->pos++;
        }
    }
    
    s_json *node = create_node(S_JSON_TYPE_NUMBER);
    if (!node) return NULL;
    
    char *num_str = strndup(state->json + start, state->pos - start);
    if (is_float) {
        node->value.d = atof(num_str);
    } else {
        node->value.i = atoi(num_str);
        node->value.d = (double)node->value.i;
    }
    free(num_str);
    
    return node;
}

static s_json* parse_null(parser_state *state) {
    if (state->pos + 4 <= state->len && 
        strncmp(state->json + state->pos, "null", 4) == 0) {
        state->pos += 4;
        return create_node(S_JSON_TYPE_NULL);
    }
    return NULL;
}

static s_json* parse_bool(parser_state *state) {
    if (state->pos + 4 <= state->len && 
        strncmp(state->json + state->pos, "true", 4) == 0) {
        state->pos += 4;
        s_json *node = create_node(S_JSON_TYPE_BOOL);
        if (node) node->value.b = true;
        return node;
    }
    
    if (state->pos + 5 <= state->len && 
        strncmp(state->json + state->pos, "false", 5) == 0) {
        state->pos += 5;
        s_json *node = create_node(S_JSON_TYPE_BOOL);
        if (node) node->value.b = false;
        return node;
    }
    
    return NULL;
}

static s_json* parse_array(parser_state *state) {
    if (state->json[state->pos] != '[') return NULL;
    state->pos++;
    
    s_json *array = create_node(S_JSON_TYPE_ARRAY);
    if (!array) return NULL;
    
    skip_whitespace(state);
    
    if (state->json[state->pos] == ']') {
        state->pos++;
        return array;
    }
    
    s_json *head = NULL, *tail = NULL;
    
    while (state->pos < state->len) {
        skip_whitespace(state);
        
        s_json *elem = parse_value(state);
        if (!elem) {
            s_json_free(array);
            return NULL;
        }
        
        if (!head) {
            head = tail = elem;
        } else {
            tail->next = elem;
            tail = elem;
        }
        array->count++;
        
        skip_whitespace(state);
        
        if (state->json[state->pos] == ']') {
            state->pos++;
            break;
        }
        
        if (state->json[state->pos] != ',') {
            s_json_free(array);
            return NULL;
        }
        state->pos++;
    }
    
    array->value.array = head;
    return array;
}

static s_json* parse_object(parser_state *state) {
    if (state->json[state->pos] != '{') return NULL;
    state->pos++;
    
    s_json *object = create_node(S_JSON_TYPE_OBJECT);
    if (!object) return NULL;
    
    skip_whitespace(state);
    
    if (state->json[state->pos] == '}') {
        state->pos++;
        return object;
    }
    
    s_json *head = NULL, *tail = NULL;
    
    while (state->pos < state->len) {
        skip_whitespace(state);
        
        char *key = parse_string(state);
        if (!key) {
            s_json_free(object);
            return NULL;
        }
        
        skip_whitespace(state);
        
        if (state->json[state->pos] != ':') {
            free(key);
            s_json_free(object);
            return NULL;
        }
        state->pos++;
        
        skip_whitespace(state);
        
        s_json *value = parse_value(state);
        if (!value) {
            free(key);
            s_json_free(object);
            return NULL;
        }
        
        value->key = key;
        
        if (!head) {
            head = tail = value;
        } else {
            tail->next = value;
            tail = value;
        }
        object->count++;
        
        skip_whitespace(state);
        
        if (state->json[state->pos] == '}') {
            state->pos++;
            break;
        }
        
        if (state->json[state->pos] != ',') {
            s_json_free(object);
            return NULL;
        }
        state->pos++;
    }
    
    object->value.object = head;
    return object;
}

static s_json* parse_value(parser_state *state) {
    skip_whitespace(state);
    
    if (state->pos >= state->len) return NULL;
    
    char c = state->json[state->pos];
    
    if (c == '"') {
        char *str = parse_string(state);
        if (!str) return NULL;
        s_json *node = create_node(S_JSON_TYPE_STRING);
        if (!node) {
            free(str);
            return NULL;
        }
        node->value.s = str;
        return node;
    }
    
    if (c == '{') return parse_object(state);
    if (c == '[') return parse_array(state);
    if (c == 't' || c == 'f') return parse_bool(state);
    if (c == 'n') return parse_null(state);
    if (c == '-' || isdigit(c)) return parse_number(state);
    
    return NULL;
}

static s_json* s_json_parse(char *json) {
    if (!json) return NULL;
    
    parser_state state = {
        .json = json,
        .pos = 0,
        .len = strlen(json)
    };
    
    return parse_value(&state);
}

static void s_json_free(s_json *j) {
    if (!j) return;
    
    if (j->key) free(j->key);
    
    switch (j->type) {
        case S_JSON_TYPE_STRING:
            if (j->value.s) free(j->value.s);
            break;
            
        case S_JSON_TYPE_ARRAY:
        case S_JSON_TYPE_OBJECT: {
            s_json *current = (j->type == S_JSON_TYPE_ARRAY) ? 
                              j->value.array : j->value.object;
            while (current) {
                s_json *next = current->next;
                s_json_free(current);
                current = next;
            }
            break;
        }
        
        default:
            break;
    }
    
    free(j);
}

static s_json* s_json_get(s_json *j, char *key) {
    if (!j || !key || j->type != S_JSON_TYPE_OBJECT) return NULL;
    
    s_json *current = j->value.object;
    while (current) {
        if (current->key && strcmp(current->key, key) == 0) {
            return current;
        }
        current = current->next;
    }
    
    return NULL;
}

static s_json* s_json_array_get(s_json *j, int index) {
    if (!j || j->type != S_JSON_TYPE_ARRAY || index < 0 || index >= j->count) {
        return NULL;
    }
    
    s_json *current = j->value.array;
    for (int i = 0; i < index && current; i++) {
        current = current->next;
    }
    
    return current;
}

static const char* s_json_get_string(s_json *j) {
    return (j && j->type == S_JSON_TYPE_STRING) ? j->value.s : NULL;
}

static double s_json_get_number(s_json *j) {
    return (j && j->type == S_JSON_TYPE_NUMBER) ? j->value.d : 0.0;
}

static int s_json_get_int(s_json *j) {
    return (j && j->type == S_JSON_TYPE_NUMBER) ? j->value.i : 0;
}

static bool s_json_get_bool(s_json *j) {
    return (j && j->type == S_JSON_TYPE_BOOL) ? j->value.b : false;
}

static void s_json_print(s_json *j, int indent) {
    if (!j) return;
    
    for (int i = 0; i < indent; i++) printf("  ");
    
    if (j->key) printf("\"%s\": ", j->key);
    
    switch (j->type) {
        case S_JSON_TYPE_NULL:
            printf("null\n");
            break;
        case S_JSON_TYPE_BOOL:
            printf("%s\n", j->value.b ? "true" : "false");
            break;
        case S_JSON_TYPE_NUMBER:
            printf("%g\n", j->value.d);
            break;
        case S_JSON_TYPE_STRING:
            printf("\"%s\"\n", j->value.s);
            break;
        case S_JSON_TYPE_ARRAY:
            printf("[\n");
            {
                s_json *current = j->value.array;
                while (current) {
                    s_json_print(current, indent + 1);
                    current = current->next;
                }
            }
            for (int i = 0; i < indent; i++) printf("  ");
            printf("]\n");
            break;
        case S_JSON_TYPE_OBJECT:
            printf("{\n");
            {
                s_json *current = j->value.object;
                while (current) {
                    s_json_print(current, indent + 1);
                    current = current->next;
                }
            }
            for (int i = 0; i < indent; i++) printf("  ");
            printf("}\n");
            break;
    }
}

#endif // S_JSON_H
