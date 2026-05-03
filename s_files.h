/**
 * Syphax - Ougi Washi
 *
 * Simple cross-platform file and directory utilities for C.
 *
 * MIT License
 * Copyright (c) 2025-2026 Jed - Ougi Washi https://github.com/ougi-washi/Syphax
 */

#ifndef S_FILES_H
#define S_FILES_H

#include "s_types.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <limits.h>

#ifdef _WIN32
    #include <windows.h>
    #include <direct.h>
    #include <io.h>
    #include <sys/stat.h>
    #define S_PATH_SEPARATOR '\\'
    #define S_PATH_SEPARATOR_STR "\\"
    typedef struct _stat s_stat_t;
    #define s_stat _stat
    #ifndef S_ISDIR
        #define S_ISDIR(m) (((m) & _S_IFDIR) != 0)
    #endif
    #ifndef S_ISREG
        #define S_ISREG(m) (((m) & _S_IFREG) != 0)
    #endif
#else
    #include <sys/stat.h>
    #include <dirent.h>
    #include <unistd.h>
    #define S_PATH_SEPARATOR '/'
    #define S_PATH_SEPARATOR_STR "/"
    typedef struct stat s_stat_t;
    int lstat(const char* path, struct stat* buf);
    #define s_stat stat
    #define s_lstat lstat
    #ifndef S_ISLNK
        #define S_ISLNK(m) (((m) & S_IFMT) == S_IFLNK)
    #endif
#endif

typedef enum {
    S_FILE_OK = 0,
    S_FILE_ERR_NOT_FOUND,
    S_FILE_ERR_IO,
    S_FILE_ERR_OOM,
    S_FILE_ERR_INVALID,
    S_FILE_ERR_PERMS
} s_file_error;

typedef struct {
    b8 ok;
    s_file_error code;
} s_file_result;

#if !defined(SYPHAX_STATIC)
b8 s_path_is_sep(char _c);
b8 s_path_exists(const char* _path);
b8 s_path_is_file(const char* _path);
b8 s_path_is_dir(const char* _path);
b8 s_file_exists(const char* _path);
b8 s_directory_exists(const char* _path);
const char* s_path_filename(const char* _path);
const char* s_path_extension(const char* _path);
b8 s_path_parent(const char* _path, char* _out_buf, sz _out_cap);
b8 s_path_join(char* _out_buf, sz _out_cap, const char* _a, const char* _b);
b8 s_directory_create(const char* _path);
b8 s_directory_remove(const char* _path);
b8 s_directory_remove_recursive(const char* _path);
b8 s_file_read(const char* _path, char** _out_data, sz* _out_size);
b8 s_file_read_binary(const char* _path, u8** _out_data, sz* _out_size);
b8 s_file_write(const char* _path, const char* _data, sz _size);
b8 s_file_write_binary(const char* _path, const void* _data, sz _size);
b8 s_file_append(const char* _path, const char* _data, sz _size);
b8 s_file_remove(const char* _path);
b8 s_file_copy(const char* _src, const char* _dst, b8 _overwrite);
b8 s_file_move(const char* _src, const char* _dst, b8 _overwrite);
b8 s_file_size(const char* _path, sz* _out_size);
b8 s_file_mtime(const char* _path, time_t* _out_mtime);
#endif

#if defined(SYPHAX_STATIC) || defined(SYPHAX_IMPLEMENTATION)
#if defined(SYPHAX_STATIC)
#define S_FILES_DEF static inline
#define S_FILES_PRIV static inline
#else
#define S_FILES_DEF
#define S_FILES_PRIV static
#endif

S_FILES_DEF b8 s_path_is_sep(char _c) {
    return _c == '/' || _c == '\\';
}

S_FILES_PRIV char* s_files_strdup(const char* _str) {
    if (_str == NULL) return NULL;
    const sz len = strlen(_str);
    char* out = (char*)s_malloc(len + 1);
    if (out == NULL) return NULL;
    memcpy(out, _str, len + 1);
    return out;
}

S_FILES_PRIV b8 s_path_is_dangerous_remove_root(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return true;
    sz len = strlen(_path);
    while (len > 1 && s_path_is_sep(_path[len - 1])) len--;
    if ((len == 1 && _path[0] == '.') ||
        (len == 2 && _path[0] == '.' && _path[1] == '.')) {
        return true;
    }
    for (sz i = 0; i < len; ) {
        while (i < len && s_path_is_sep(_path[i])) i++;
        const sz start = i;
        while (i < len && !s_path_is_sep(_path[i])) i++;
        if (i - start == 2 && _path[start] == '.' && _path[start + 1] == '.') return true;
    }
#ifdef _WIN32
    if (len == 1 && s_path_is_sep(_path[0])) return true;
    if (len >= 2 && s_path_is_sep(_path[0]) && s_path_is_sep(_path[1])) {
        sz components = 0;
        b8 in_component = false;
        for (sz i = 2; i < len; ++i) {
            if (s_path_is_sep(_path[i])) {
                if (in_component) components++;
                in_component = false;
            } else {
                in_component = true;
            }
        }
        if (in_component) components++;
        if (components <= 2) return true;
    }
    if (len >= 2 &&
        ((_path[0] >= 'A' && _path[0] <= 'Z') || (_path[0] >= 'a' && _path[0] <= 'z')) &&
        _path[1] == ':') {
        if (len == 2) return true;
    }
#else
    if (len == 1 && s_path_is_sep(_path[0])) return true;
#endif
    return false;
}

// Path
S_FILES_DEF b8 s_path_exists(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    s_stat_t st;
    return s_stat(_path, &st) == 0;
}

S_FILES_DEF b8 s_path_is_file(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    s_stat_t st;
    if (s_stat(_path, &st) != 0) return false;
    return S_ISREG(st.st_mode) != 0;
}

S_FILES_DEF b8 s_path_is_dir(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    s_stat_t st;
    if (s_stat(_path, &st) != 0) return false;
    return S_ISDIR(st.st_mode) != 0;
}

S_FILES_DEF b8 s_file_exists(const char* _path) {
    return s_path_is_file(_path);
}

S_FILES_DEF b8 s_directory_exists(const char* _path) {
    return s_path_is_dir(_path);
}

S_FILES_DEF const char* s_path_filename(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return _path;
    sz len = strlen(_path);
    while (len > 0 && s_path_is_sep(_path[len - 1])) {
#ifdef _WIN32
        if (len == 3 && _path[1] == ':' && s_path_is_sep(_path[2])) break;
#else
        if (len == 1 && s_path_is_sep(_path[0])) break;
#endif
        len--;
    }
    if (len == 0) return _path;
    sz i = len;
    while (i > 0 && !s_path_is_sep(_path[i - 1])) i--;
    return _path + i;
}

S_FILES_DEF const char* s_path_extension(const char* _path) {
    const char* name = s_path_filename(_path);
    if (name == NULL) return NULL;
    const char* dot = strrchr(name, '.');
    if (dot == NULL || dot == name) return NULL;
    return dot;
}

S_FILES_DEF b8 s_path_parent(const char* _path, char* _out_buf, sz _out_cap) {
    if (_path == NULL || _out_buf == NULL || _out_cap == 0) return false;
    sz len = strlen(_path);
    if (len == 0) return false;
    while (len > 0 && s_path_is_sep(_path[len - 1])) {
#ifdef _WIN32
        if (len == 3 && _path[1] == ':' && s_path_is_sep(_path[2])) break;
#else
        if (len == 1 && s_path_is_sep(_path[0])) break;
#endif
        len--;
    }
    if (len == 0) return false;
    sz i = len;
    while (i > 0 && !s_path_is_sep(_path[i - 1])) i--;
    if (i == 0) return false;
#ifdef _WIN32
    if (i == 3 && _path[1] == ':' && s_path_is_sep(_path[2])) {
        if (_out_cap < 4) return false;
        memcpy(_out_buf, _path, 3);
        _out_buf[3] = '\0';
        return true;
    }
#endif
    if (i == 1 && s_path_is_sep(_path[0])) {
        if (_out_cap < 2) return false;
        _out_buf[0] = S_PATH_SEPARATOR;
        _out_buf[1] = '\0';
        return true;
    }
    if (i - 1 >= _out_cap) return false;
    memcpy(_out_buf, _path, i - 1);
    _out_buf[i - 1] = '\0';
    return true;
}

S_FILES_DEF b8 s_path_join(char* _out_buf, sz _out_cap, const char* _a, const char* _b) {
    if (_out_buf == NULL || _out_cap == 0) return false;
    if (_a == NULL || _a[0] == '\0') {
        if (_b == NULL) return false;
        const sz blen = strlen(_b);
        if (blen + 1 > _out_cap) return false;
        memcpy(_out_buf, _b, blen);
        _out_buf[blen] = '\0';
        return true;
    }
    if (_b == NULL || _b[0] == '\0') {
        const sz alen = strlen(_a);
        if (alen + 1 > _out_cap) return false;
        memcpy(_out_buf, _a, alen);
        _out_buf[alen] = '\0';
        return true;
    }
    const sz alen = strlen(_a);
    const sz blen = strlen(_b);
    const b8 a_sep = s_path_is_sep(_a[alen - 1]);
    const b8 b_sep = s_path_is_sep(_b[0]);
    if (alen > SIZE_MAX - blen) return false;
    sz total = alen + blen;
    if (a_sep && b_sep) {
        if (total > SIZE_MAX - 1) return false;
    } else if (!a_sep && !b_sep) {
        if (total > SIZE_MAX - 2) return false;
        total += 2;
    } else {
        if (total > SIZE_MAX - 1) return false;
        total += 1;
    }
    if (total > _out_cap) return false;
    memcpy(_out_buf, _a, alen);
    sz pos = alen;
    if (a_sep && b_sep) {
        _out_buf[pos - 1] = S_PATH_SEPARATOR;
        memcpy(_out_buf + pos, _b + 1, blen - 1);
        pos += blen - 1;
    } else if (!a_sep && !b_sep) {
        _out_buf[pos++] = S_PATH_SEPARATOR;
        memcpy(_out_buf + pos, _b, blen);
        pos += blen;
    } else {
        memcpy(_out_buf + pos, _b, blen);
        pos += blen;
    }
    _out_buf[pos] = '\0';
    return true;
}

S_FILES_PRIV b8 s_directory_create_one(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    if (s_directory_exists(_path)) return true;
#ifdef _WIN32
    if (_mkdir(_path) == 0) return true;
    if (errno == EEXIST) return true;
    return false;
#else
    if (mkdir(_path, 0755) == 0) return true;
    if (errno == EEXIST) return true;
    return false;
#endif
}

// Directory
S_FILES_DEF b8 s_directory_create(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    if (s_directory_exists(_path)) return true;
    char* tmp = s_files_strdup(_path);
    if (tmp == NULL) return false;
    char* p = tmp;
#ifdef _WIN32
    if (p[0] != '\0' && p[1] == ':') {
        p += 2;
        if (s_path_is_sep(*p)) p++;
    } else if (s_path_is_sep(p[0]) && s_path_is_sep(p[1])) {
        p += 2;
        while (*p && !s_path_is_sep(*p)) p++;
        if (s_path_is_sep(*p)) p++;
        while (*p && !s_path_is_sep(*p)) p++;
        if (s_path_is_sep(*p)) p++;
    } else if (s_path_is_sep(*p)) {
        p++;
    }
#else
    if (s_path_is_sep(*p)) p++;
#endif
    for (; *p; p++) {
        if (s_path_is_sep(*p)) {
            *p = '\0';
            if (!s_directory_create_one(tmp)) {
                s_free(tmp);
                return false;
            }
            *p = S_PATH_SEPARATOR;
        }
    }
    if (!s_directory_create_one(tmp)) {
        s_free(tmp);
        return false;
    }
    s_free(tmp);
    return true;
}

S_FILES_DEF b8 s_directory_remove(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
#ifdef _WIN32
    return _rmdir(_path) == 0;
#else
    return rmdir(_path) == 0;
#endif
}

S_FILES_DEF b8 s_directory_remove_recursive(const char* _path) {
    if (s_path_is_dangerous_remove_root(_path)) return false;
#ifdef _WIN32
    DWORD root_attrs = GetFileAttributesA(_path);
    if (root_attrs == INVALID_FILE_ATTRIBUTES) return false;
    if ((root_attrs & FILE_ATTRIBUTE_DIRECTORY) == 0) return false;
    if ((root_attrs & FILE_ATTRIBUTE_REPARSE_POINT) != 0) return false;
    char pattern[MAX_PATH];
    if (!s_path_join(pattern, sizeof(pattern), _path, "*")) return false;
    WIN32_FIND_DATAA data;
    HANDLE h = FindFirstFileA(pattern, &data);
    if (h == INVALID_HANDLE_VALUE) return false;
    do {
        const char* name = data.cFileName;
        if (strcmp(name, ".") == 0 || strcmp(name, "..") == 0) continue;
        char full[MAX_PATH];
        if (!s_path_join(full, sizeof(full), _path, name)) { FindClose(h); return false; }
        if (data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            if (data.dwFileAttributes & FILE_ATTRIBUTE_REPARSE_POINT) {
                if (!RemoveDirectoryA(full)) { FindClose(h); return false; }
            } else if (!s_directory_remove_recursive(full)) {
                FindClose(h);
                return false;
            }
        } else {
            if (!DeleteFileA(full)) { FindClose(h); return false; }
        }
    } while (FindNextFileA(h, &data) != 0);
    FindClose(h);
    return RemoveDirectoryA(_path) != 0;
#else
    s_stat_t root_st;
    if (s_lstat(_path, &root_st) != 0) return false;
    if (S_ISLNK(root_st.st_mode) || !S_ISDIR(root_st.st_mode)) return false;
    DIR* dir = opendir(_path);
    if (dir == NULL) return false;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        sz base_len = strlen(_path);
        sz name_len = strlen(ent->d_name);
        if (base_len > SIZE_MAX - name_len || base_len + name_len > SIZE_MAX - 2) {
            closedir(dir);
            return false;
        }
        sz full_len = base_len + name_len + 2;
        char* full = (char*)s_malloc(full_len);
        if (full == NULL) { closedir(dir); return false; }
        if (!s_path_join(full, full_len, _path, ent->d_name)) {
            s_free(full);
            closedir(dir);
            return false;
        }
        s_stat_t st;
        if (s_lstat(full, &st) != 0) {
            s_free(full);
            closedir(dir);
            return false;
        }
        if (S_ISDIR(st.st_mode) && !S_ISLNK(st.st_mode)) {
            if (!s_directory_remove_recursive(full)) { s_free(full); closedir(dir); return false; }
        } else {
            if (remove(full) != 0) { s_free(full); closedir(dir); return false; }
        }
        s_free(full);
    }
    if (closedir(dir) != 0) return false;
    return rmdir(_path) == 0;
#endif
}

// File
S_FILES_PRIV b8 s_file_read_alloc(const char* _path, void** _out_data, sz* _out_size, b8 _nul_terminate) {
    if (_path == NULL || _path[0] == '\0' || _out_data == NULL) return false;
    FILE* f = fopen(_path, "rb");
    if (f == NULL) return false;
    s_stat_t st;
    const uintmax_t max_size = _nul_terminate ? (uintmax_t)(SIZE_MAX - 1) : (uintmax_t)SIZE_MAX;
    if (s_stat(_path, &st) != 0 || st.st_size < 0 || (uintmax_t)st.st_size > max_size) { fclose(f); return false; }
    const sz size = (sz)st.st_size;
    const sz alloc_size = size + (_nul_terminate ? 1u : 0u);
    u8* data = (u8*)s_malloc(alloc_size);
    if (data == NULL && alloc_size > 0) { fclose(f); return false; }
    sz read_size = 0;
    if (size > 0) read_size = fread(data, 1, size, f);
    b8 close_ok = fclose(f) == 0;
    if (read_size != size || !close_ok) { s_free(data); return false; }
    if (_nul_terminate) data[size] = '\0';
    *_out_data = data;
    if (_out_size != NULL) *_out_size = size;
    return true;
}

S_FILES_DEF b8 s_file_read(const char* _path, char** _out_data, sz* _out_size) {
    void* data = NULL;
    if (!s_file_read_alloc(_path, &data, _out_size, true)) return false;
    *_out_data = (char*)data;
    return true;
}

S_FILES_DEF b8 s_file_read_binary(const char* _path, u8** _out_data, sz* _out_size) {
    void* data = NULL;
    if (!s_file_read_alloc(_path, &data, _out_size, false)) return false;
    *_out_data = data;
    return true;
}

S_FILES_PRIV b8 s_file_write_mode(const char* _path, const void* _data, sz _size, const char* _mode) {
    if (_path == NULL || _path[0] == '\0') return false;
    if (_size > 0 && _data == NULL) return false;
    FILE* f = fopen(_path, _mode);
    if (f == NULL) return false;
    b8 ok = true;
    if (_size > 0 && fwrite(_data, 1, _size, f) != _size) ok = false;
    if (fclose(f) != 0) ok = false;
    return ok;
}

S_FILES_DEF b8 s_file_write(const char* _path, const char* _data, sz _size) {
    return s_file_write_mode(_path, _data, _size, "wb");
}

S_FILES_DEF b8 s_file_write_binary(const char* _path, const void* _data, sz _size) {
    return s_file_write_mode(_path, _data, _size, "wb");
}

S_FILES_DEF b8 s_file_append(const char* _path, const char* _data, sz _size) {
    return s_file_write_mode(_path, _data, _size, "ab");
}

S_FILES_DEF b8 s_file_remove(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    return remove(_path) == 0;
}

S_FILES_DEF b8 s_file_copy(const char* _src, const char* _dst, b8 _overwrite) {
    if (_src == NULL || _dst == NULL || _src[0] == '\0' || _dst[0] == '\0') return false;
    if (!_overwrite && s_path_exists(_dst)) return false;
    FILE* fsrc = fopen(_src, "rb");
    if (fsrc == NULL) return false;
    FILE* fdst = fopen(_dst, "wb");
    if (fdst == NULL) { fclose(fsrc); return false; }
    unsigned char buf[8192];
    sz nread = 0;
    while ((nread = fread(buf, 1, sizeof(buf), fsrc)) > 0) {
        if (fwrite(buf, 1, nread, fdst) != nread) { fclose(fsrc); fclose(fdst); return false; }
    }
    b8 ok = true;
    if (ferror(fsrc)) ok = false;
    if (fclose(fsrc) != 0) ok = false;
    if (fclose(fdst) != 0) ok = false;
    return ok;
}

S_FILES_DEF b8 s_file_move(const char* _src, const char* _dst, b8 _overwrite) {
    if (_src == NULL || _dst == NULL || _src[0] == '\0' || _dst[0] == '\0') return false;
    if (!_overwrite && s_path_exists(_dst)) return false;
    if (rename(_src, _dst) == 0) return true;

    const sz dst_len = strlen(_dst);
    if (dst_len > SIZE_MAX - 64) return false;
    char* tmp = (char*)s_malloc(dst_len + 64);
    if (tmp == NULL) return false;

    b8 copied = false;
    for (u32 i = 0; i < 32u; ++i) {
        int n = snprintf(tmp, dst_len + 64, "%s.syphax_tmp_%lu_%u", _dst, (unsigned long)time(NULL), i);
        if (n <= 0 || (sz)n >= dst_len + 64) {
            s_free(tmp);
            return false;
        }
        if (!s_path_exists(tmp)) {
            copied = s_file_copy(_src, tmp, false);
            if (!copied) s_file_remove(tmp);
            break;
        }
    }
    if (!copied) {
        s_free(tmp);
        return false;
    }
    if (!_overwrite && s_path_exists(_dst)) {
        s_file_remove(tmp);
        s_free(tmp);
        return false;
    }

#ifdef _WIN32
    DWORD move_flags = _overwrite ? MOVEFILE_REPLACE_EXISTING : 0;
    if (!MoveFileExA(tmp, _dst, move_flags)) {
        s_file_remove(tmp);
        s_free(tmp);
        return false;
    }
#else
    if (rename(tmp, _dst) != 0) {
        s_file_remove(tmp);
        s_free(tmp);
        return false;
    }
#endif
    s_free(tmp);
    return s_file_remove(_src);
}

S_FILES_DEF b8 s_file_size(const char* _path, sz* _out_size) {
    if (_path == NULL || _path[0] == '\0' || _out_size == NULL) return false;
    s_stat_t st;
    if (s_stat(_path, &st) != 0) return false;
    if (st.st_size < 0 || (uintmax_t)st.st_size > (uintmax_t)SIZE_MAX) return false;
    *_out_size = (sz)st.st_size;
    return true;
}

S_FILES_DEF b8 s_file_mtime(const char* _path, time_t* _out_mtime) {
    if (_path == NULL || _path[0] == '\0' || _out_mtime == NULL) return false;
    s_stat_t st;
    if (s_stat(_path, &st) != 0) return false;
    *_out_mtime = st.st_mtime;
    return true;
}

#undef S_FILES_DEF
#undef S_FILES_PRIV
#endif

#endif // S_FILES_H
