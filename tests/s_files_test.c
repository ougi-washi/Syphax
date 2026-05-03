/**
 * Syphax - Ougi Washi
 *
 * Tests for s_files.h
 */

#define SYPHAX_STATIC
#include "../s_files.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#ifndef _WIN32
int symlink(const char* target, const char* linkpath);
#endif

static void require_true(b8 cond, const char* msg) {
    if (!cond) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

static void require_str_eq(const char* a, const char* b, const char* msg) {
    if ((a == NULL && b != NULL) || (a != NULL && b == NULL) || (a != NULL && b != NULL && strcmp(a, b) != 0)) {
        fprintf(stderr, "FAIL: %s (got '%s', expected '%s')\n", msg, a ? a : "(null)", b ? b : "(null)");
        exit(1);
    }
}

static void require_mem_eq(const void* a, const void* b, sz size, const char* msg) {
    if (size > 0 && memcmp(a, b, size) != 0) {
        fprintf(stderr, "FAIL: %s\n", msg);
        exit(1);
    }
}

int main(void) {
    const char* base = "tests";
    char tmp_root[512];
    require_true(s_path_join(tmp_root, sizeof(tmp_root), base, "s_files_tmp"), "path_join tmp_root");
    require_true(s_path_is_dangerous_remove_root(""), "empty remove root rejected");
    require_true(s_path_is_dangerous_remove_root("."), "dot remove root rejected");
    require_true(s_path_is_dangerous_remove_root("./"), "dot slash remove root rejected");
    require_true(s_path_is_dangerous_remove_root(".."), "parent remove root rejected");
    require_true(s_path_is_dangerous_remove_root("../"), "parent slash remove root rejected");
#ifdef _WIN32
    require_true(s_path_is_dangerous_remove_root("\\"), "windows root rejected");
    require_true(s_path_is_dangerous_remove_root("C:"), "windows drive current root rejected");
    require_true(s_path_is_dangerous_remove_root("C:\\"), "windows drive root rejected");
    require_true(s_path_is_dangerous_remove_root("\\\\server\\share"), "windows UNC share root rejected");
#else
    require_true(s_path_is_dangerous_remove_root("/"), "posix root rejected");
    require_true(s_path_is_dangerous_remove_root("//"), "posix repeated root rejected");
    require_true(s_path_is_dangerous_remove_root("/tmp/.."), "posix normalized root-like path rejected");
#endif

    if (s_directory_exists(tmp_root)) {
        s_directory_remove_recursive(tmp_root);
    }

    char nested[512];
    require_true(s_path_join(nested, sizeof(nested), tmp_root, "a/b/c"), "path_join nested");
    require_true(s_directory_create(nested), "directory_create nested");
    require_true(s_directory_exists(tmp_root), "directory_exists root");
    require_true(s_path_exists(tmp_root), "path_exists root");
    require_true(s_path_is_dir(tmp_root), "path_is_dir root");
    require_true(!s_path_is_file(tmp_root), "path_is_file root false");

    char parent_buf[512];
    require_true(s_path_parent(nested, parent_buf, sizeof(parent_buf)), "path_parent nested");
    require_true(s_directory_exists(parent_buf), "directory_exists parent");

    char file_path[512];
    require_true(s_path_join(file_path, sizeof(file_path), tmp_root, "file.txt"), "path_join file");
    const char* text = "hello";
    require_true(s_file_write(file_path, text, (sz)strlen(text)), "file_write text");
    require_true(s_file_exists(file_path), "file_exists");
    require_true(s_path_is_file(file_path), "path_is_file file");

    sz size = 0;
    require_true(s_file_size(file_path, &size), "file_size");
    require_true(size == 5, "file_size matches");

    char* read_text = NULL;
    sz read_size = 0;
    require_true(s_file_read(file_path, &read_text, &read_size), "file_read");
    require_true(read_size == 5, "file_read size");
    require_str_eq(read_text, "hello", "file_read content");
    s_free(read_text);

    const char* more = " world";
    require_true(s_file_append(file_path, more, (sz)strlen(more)), "file_append");
    require_true(s_file_read(file_path, &read_text, &read_size), "file_read appended");
    require_str_eq(read_text, "hello world", "file_append content");
    s_free(read_text);

    unsigned char bin_data[8] = { 0, 1, 2, 3, 4, 5, 6, 7 };
    char bin_path[512];
    require_true(s_path_join(bin_path, sizeof(bin_path), tmp_root, "data.bin"), "path_join bin");
    require_true(s_file_write_binary(bin_path, bin_data, sizeof(bin_data)), "file_write_binary");

    u8* read_bin = NULL;
    sz read_bin_size = 0;
    require_true(s_file_read_binary(bin_path, &read_bin, &read_bin_size), "file_read_binary");
    require_true(read_bin_size == sizeof(bin_data), "file_read_binary size");
    require_mem_eq(read_bin, bin_data, sizeof(bin_data), "file_read_binary content");
    s_free(read_bin);

    char copy_path[512];
    require_true(s_path_join(copy_path, sizeof(copy_path), tmp_root, "copy.bin"), "path_join copy");
    require_true(s_file_copy(bin_path, copy_path, true), "file_copy overwrite");
    require_true(s_file_read_binary(copy_path, &read_bin, &read_bin_size), "file_read_binary copy");
    require_mem_eq(read_bin, bin_data, sizeof(bin_data), "file_copy content");
    s_free(read_bin);

    char move_path[512];
    require_true(s_path_join(move_path, sizeof(move_path), tmp_root, "moved.bin"), "path_join move");
    require_true(s_file_move(copy_path, move_path, true), "file_move");
    require_true(!s_path_exists(copy_path), "file_move removed source");
    require_true(s_file_exists(move_path), "file_move dest exists");

    char missing_path[512];
    require_true(s_path_join(missing_path, sizeof(missing_path), tmp_root, "missing.bin"), "path_join missing");
    require_true(!s_file_move(missing_path, move_path, true), "file_move missing source fails");
    require_true(s_file_read_binary(move_path, &read_bin, &read_bin_size), "file_move failed overwrite read");
    require_mem_eq(read_bin, bin_data, sizeof(bin_data), "file_move failed overwrite preserved dest");
    s_free(read_bin);

    char second_src[512];
    require_true(s_path_join(second_src, sizeof(second_src), tmp_root, "second.bin"), "path_join second_src");
    require_true(s_file_write_binary(second_src, "new", 3), "second_src write");
    require_true(!s_file_move(second_src, move_path, false), "file_move no-overwrite fails");
    require_true(s_file_exists(second_src), "file_move no-overwrite preserves source");
    require_true(s_file_read_binary(move_path, &read_bin, &read_bin_size), "file_move no-overwrite read dest");
    require_mem_eq(read_bin, bin_data, sizeof(bin_data), "file_move no-overwrite preserves dest");
    s_free(read_bin);
    require_true(s_file_remove(second_src), "remove second_src");

    const char* name = s_path_filename("foo/bar/baz.txt");
    require_str_eq(name, "baz.txt", "path_filename");
    const char* ext = s_path_extension("foo/bar/baz.txt");
    require_str_eq(ext, ".txt", "path_extension");
    require_true(s_path_extension("foo/bar/.hidden") == NULL, "path_extension hidden");

    char join_buf[512];
    require_true(s_path_join(join_buf, sizeof(join_buf), "foo/", "/bar"), "path_join trim");
    char expected_join[16];
    expected_join[0] = 'f';
    expected_join[1] = 'o';
    expected_join[2] = 'o';
    expected_join[3] = S_PATH_SEPARATOR;
    expected_join[4] = 'b';
    expected_join[5] = 'a';
    expected_join[6] = 'r';
    expected_join[7] = '\0';
    require_str_eq(join_buf, expected_join, "path_join trim result");

    require_true(s_file_remove(file_path), "file_remove");
    require_true(!s_path_exists(file_path), "file_remove gone");

#ifndef _WIN32
    char outside_dir[512];
    require_true(s_path_join(outside_dir, sizeof(outside_dir), base, "s_files_outside"), "path_join outside");
    if (s_directory_exists(outside_dir)) {
        s_directory_remove_recursive(outside_dir);
    }
    require_true(s_directory_create(outside_dir), "directory_create outside");
    char outside_file[512];
    require_true(s_path_join(outside_file, sizeof(outside_file), outside_dir, "keep.txt"), "path_join outside file");
    require_true(s_file_write(outside_file, "keep", 4), "outside file write");

    char link_path[512];
    require_true(s_path_join(link_path, sizeof(link_path), tmp_root, "linked_dir"), "path_join symlink");
    if (symlink(outside_dir, link_path) == 0) {
        require_true(s_directory_remove_recursive(tmp_root), "recursive remove skips symlink target");
        require_true(s_file_exists(outside_file), "recursive remove preserved symlink target");
        require_true(s_directory_remove_recursive(outside_dir), "remove outside after symlink test");
        require_true(s_directory_create(tmp_root), "recreate tmp root after symlink test");
    } else {
        s_directory_remove_recursive(outside_dir);
    }
#endif

    char empty_dir[512];
    require_true(s_path_join(empty_dir, sizeof(empty_dir), tmp_root, "empty"), "path_join empty");
    require_true(s_directory_create(empty_dir), "directory_create empty");
    require_true(s_directory_remove(empty_dir), "directory_remove empty");

    require_true(s_directory_remove_recursive(tmp_root), "directory_remove_recursive root");
    require_true(!s_path_exists(tmp_root), "directory_remove_recursive gone");

    printf("All s_files tests passed.\n");
    return 0;
}
