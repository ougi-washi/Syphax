/**
 * Syphax - Ougi Washi
 *
 * Tests for s_json.h strict parsing and writing.
 */

#define SYPHAX_STATIC
#include "../s_json.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static void require_true(b8 cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

static void require_parse_fail(const char* text, s_json_error_code code, const char* msg) {
    s_json_error err = {0};
    s_json* json = s_json_parse_with_error(text, &err);
    if (json != NULL) {
        s_json_free(json);
        fprintf(stderr, "FAIL: %s parsed unexpectedly\n", msg);
        exit(1);
    }
    if (err.code != code) {
        fprintf(stderr, "FAIL: %s got %s expected %s\n",
            msg, s_json_error_string(err.code), s_json_error_string(code));
        exit(1);
    }
}

int main(void) {
    s_json_error err = {0};
    s_json* root = s_json_parse_with_error("{\"a\":1,\"a\":2,\"text\":\"ok\"}", &err);
    require_true(root != NULL && err.code == S_JSON_OK, "parse duplicate object keys");
    require_true(s_json_count(root) == 3, "duplicates are preserved in insertion order");
    s_json* first = s_json_get(root, "a");
    i64 value = 0;
    require_true(s_json_as_i64(first, &value) && value == 1, "s_json_get returns first duplicate key");
    s_json_free(root);

    require_parse_fail("01", S_JSON_ERR_INVALID_NUMBER, "leading zero");
    require_parse_fail("1.", S_JSON_ERR_INVALID_NUMBER, "missing fraction digit");
    require_parse_fail("1e", S_JSON_ERR_INVALID_NUMBER, "missing exponent digit");
    require_parse_fail("1e+", S_JSON_ERR_INVALID_NUMBER, "missing signed exponent digit");
    require_parse_fail("1e999999", S_JSON_ERR_INVALID_NUMBER, "number overflow");
    require_parse_fail("NaN", S_JSON_ERR_UNEXPECTED_CHAR, "NaN rejected");
    require_parse_fail("Infinity", S_JSON_ERR_UNEXPECTED_CHAR, "Infinity rejected");
    require_parse_fail("\"\\uDC00\"", S_JSON_ERR_INVALID_STRING, "lone low surrogate");
    require_parse_fail("\"\\uD800x\"", S_JSON_ERR_INVALID_STRING, "invalid surrogate pair");
    require_parse_fail("\"\xC0\x80\"", S_JSON_ERR_INVALID_STRING, "overlong raw UTF-8");
    char deep[(S_JSON_MAX_DEPTH + 2) * 2 + 2];
    sz pos = 0;
    for (sz i = 0; i < S_JSON_MAX_DEPTH + 1; ++i) deep[pos++] = '[';
    deep[pos++] = '0';
    for (sz i = 0; i < S_JSON_MAX_DEPTH + 1; ++i) deep[pos++] = ']';
    deep[pos] = '\0';
    require_parse_fail(deep, S_JSON_ERR_DEPTH, "depth limit");

    s_json* num = s_json_num(NULL, NAN);
    require_true(num != NULL, "create NaN number node");
    char* text = s_json_stringify(num);
    require_true(text == NULL, "stringify rejects NaN");
    s_json_free(num);

    num = s_json_num(NULL, 1.5);
    value = 0;
    require_true(num != NULL, "create fractional number node");
    require_true(!s_json_as_i64(num, &value), "s_json_as_i64 rejects fractional number");
    s_json_free(num);

    s_json* tmp = s_json_null(NULL);
    require_true(tmp != NULL, "create temp null");
    require_true(!s_json_add(NULL, tmp), "s_json_add NULL parent returns false");
    s_json_free(tmp);
    s_json_set_name(NULL, "ignored");
    s_json_set_name_take(NULL, s_json_strdup("ignored"));
    require_true(s_json_write(NULL, NULL, 8) == 0, "s_json_write NULL buffer fails cleanly");

    char* invalid_utf8 = s_json_strdup("\xC0\x80");
    require_true(invalid_utf8 != NULL, "invalid UTF-8 string allocation");
    s_json* bad_str = s_json_str_take(NULL, invalid_utf8);
    require_true(bad_str != NULL, "create invalid UTF-8 string node");
    text = s_json_stringify(bad_str);
    require_true(text == NULL, "stringify rejects invalid UTF-8 string");
    s_json_free(bad_str);

    root = s_json_parse("[\"\\uD83D\\uDE00\",true,null,-12.5e2]");
    require_true(root != NULL, "valid strict JSON parses");
    text = s_json_stringify(root);
    require_true(text != NULL && strcmp(text, "[\"\xF0\x9F\x98\x80\",true,null,-1250]") == 0, "valid JSON stringifies");
    s_free(text);
    s_json_free(root);

    root = s_json_parse("{\"a\":[10]}");
    require_true(root != NULL, "path test parses");
    require_true(s_json_get_path(root, "a[0]") != NULL, "path key and index works");
    require_true(s_json_get_path(root, ".[0]") == NULL, "path leading dot needs key");
    s_json_free(root);

    printf("All s_json tests passed.\n");
    return 0;
}
