#define SYPHAX_STATIC
#include "s_files.h"
#include <stdio.h>
#include <string.h>

int main(void) {
    const char* root = "examples_tmp";
    if (s_directory_exists(root)) {
        s_directory_remove_recursive(root);
    }
    if (!s_directory_create(root)) {
        fprintf(stderr, "failed to create %s\n", root);
        return 1;
    }

    char path[256];
    if (!s_path_join(path, sizeof(path), root, "hello.txt")) {
        fprintf(stderr, "path_join failed\n");
        return 1;
    }

    const char* msg = "hello";
    if (!s_file_write(path, msg, (sz)strlen(msg))) {
        fprintf(stderr, "file_write failed\n");
        return 1;
    }

    char* data = NULL;
    sz size = 0;
    if (!s_file_read(path, &data, &size)) {
        fprintf(stderr, "file_read failed\n");
        return 1;
    }
    printf("read=%s\n", data);
    s_free(data);

    const char* more = " world";
    s_file_append(path, more, (sz)strlen(more));
    if (!s_file_read(path, &data, &size)) {
        fprintf(stderr, "file_read failed\n");
        return 1;
    }
    printf("appended=%s\n", data);
    s_free(data);

    s_directory_remove_recursive(root);
    return 0;
}
