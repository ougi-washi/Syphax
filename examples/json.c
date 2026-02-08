#include "s_json.h"
#include <stdio.h>
#include <stdlib.h>

int main(void) {
    s_json* doc = s_json_object("root",
        s_json_str("name", "Syphax"),
        s_json_num("version", 1.0),
        s_json_array("nums",
            s_json_int(NULL, 1),
            s_json_int(NULL, 2),
            s_json_int(NULL, 3)
        )
    );

    char* text = s_json_stringify(doc);
    if (text == NULL) {
        fprintf(stderr, "stringify failed\n");
        s_json_free(doc);
        return 1;
    }

    printf("%s\n", text);

    s_json_error err = {0};
    s_json* parsed = s_json_parse_with_error(text, &err);
    if (parsed == NULL) {
        fprintf(stderr, "parse failed at %zu:%zu\n", err.line, err.column);
        free(text);
        s_json_free(doc);
        return 1;
    }

    s_json* name = s_json_get(parsed, "name");
    if (name && name->type == S_JSON_STRING) {
        printf("name=%s\n", name->as.string);
    }

    s_json_free(parsed);
    s_json_free(doc);
    free(text);
    return 0;
}
