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
    #define s_stat stat
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

static inline b8 s_path_is_sep(char _c) {
    return _c == '/' || _c == '\\';
}

static inline char* s_files_strdup(const char* _str) {
    if (_str == NULL) return NULL;
    const sz len = strlen(_str);
    char* out = (char*)malloc(len + 1);
    if (out == NULL) return NULL;
    memcpy(out, _str, len);
    out[len] = '\0';
    return out;
}

// Path
static inline b8 s_path_exists(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    s_stat_t st;
    return s_stat(_path, &st) == 0;
}

static inline b8 s_path_is_file(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    s_stat_t st;
    if (s_stat(_path, &st) != 0) return false;
    return S_ISREG(st.st_mode) != 0;
}

static inline b8 s_path_is_dir(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    s_stat_t st;
    if (s_stat(_path, &st) != 0) return false;
    return S_ISDIR(st.st_mode) != 0;
}

static inline b8 s_file_exists(const char* _path) {
    return s_path_is_file(_path);
}

static inline b8 s_directory_exists(const char* _path) {
    return s_path_is_dir(_path);
}

static inline const char* s_path_filename(const char* _path) {
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

static inline const char* s_path_extension(const char* _path) {
    const char* name = s_path_filename(_path);
    if (name == NULL) return NULL;
    const char* dot = strrchr(name, '.');
    if (dot == NULL || dot == name) return NULL;
    return dot;
}

static inline b8 s_path_parent(const char* _path, char* _out_buf, sz _out_cap) {
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

static inline b8 s_path_join(char* _out_buf, sz _out_cap, const char* _a, const char* _b) {
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
    sz total = alen + blen + 1;
    if (a_sep && b_sep) total -= 1;
    if (!a_sep && !b_sep) total += 1;
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

static inline b8 s_directory_create_one(const char* _path) {
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
static inline b8 s_directory_create(const char* _path) {
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
                free(tmp);
                return false;
            }
            *p = S_PATH_SEPARATOR;
        }
    }
    if (!s_directory_create_one(tmp)) {
        free(tmp);
        return false;
    }
    free(tmp);
    return true;
}

static inline b8 s_directory_remove(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
#ifdef _WIN32
    return _rmdir(_path) == 0;
#else
    return rmdir(_path) == 0;
#endif
}

static inline b8 s_directory_remove_recursive(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    if (!s_directory_exists(_path)) return false;
#ifdef _WIN32
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
            if (!s_directory_remove_recursive(full)) { FindClose(h); return false; }
        } else {
            if (!DeleteFileA(full)) { FindClose(h); return false; }
        }
    } while (FindNextFileA(h, &data) != 0);
    FindClose(h);
    return RemoveDirectoryA(_path) != 0;
#else
    DIR* dir = opendir(_path);
    if (dir == NULL) return false;
    struct dirent* ent;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) continue;
        sz base_len = strlen(_path);
        sz name_len = strlen(ent->d_name);
        sz full_len = base_len + name_len + 2;
        char* full = (char*)malloc(full_len);
        if (full == NULL) { closedir(dir); return false; }
        if (!s_path_join(full, full_len, _path, ent->d_name)) {
            free(full);
            closedir(dir);
            return false;
        }
        if (s_directory_exists(full)) {
            if (!s_directory_remove_recursive(full)) { free(full); closedir(dir); return false; }
        } else {
            if (remove(full) != 0) { free(full); closedir(dir); return false; }
        }
        free(full);
    }
    closedir(dir);
    return rmdir(_path) == 0;
#endif
}

// File
static inline b8 s_file_read(const char* _path, char** _out_data, sz* _out_size) {
    if (_path == NULL || _path[0] == '\0' || _out_data == NULL) return false;
    FILE* f = fopen(_path, "rb");
    if (f == NULL) return false;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return false; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return false; }
    char* data = (char*)malloc((sz)size + 1);
    if (data == NULL) { fclose(f); return false; }
    sz read_size = 0;
    if (size > 0) read_size = fread(data, 1, (sz)size, f);
    fclose(f);
    if (read_size != (sz)size) { free(data); return false; }
    data[(sz)size] = '\0';
    *_out_data = data;
    if (_out_size != NULL) *_out_size = (sz)size;
    return true;
}

static inline b8 s_file_read_binary(const char* _path, u8** _out_data, sz* _out_size) {
    if (_path == NULL || _path[0] == '\0' || _out_data == NULL) return false;
    FILE* f = fopen(_path, "rb");
    if (f == NULL) return false;
    if (fseek(f, 0, SEEK_END) != 0) { fclose(f); return false; }
    long size = ftell(f);
    if (size < 0) { fclose(f); return false; }
    if (fseek(f, 0, SEEK_SET) != 0) { fclose(f); return false; }
    u8* data = (u8*)malloc((sz)size);
    if (data == NULL && size > 0) { fclose(f); return false; }
    sz read_size = 0;
    if (size > 0) read_size = fread(data, 1, (sz)size, f);
    fclose(f);
    if (read_size != (sz)size) { free(data); return false; }
    *_out_data = data;
    if (_out_size != NULL) *_out_size = (sz)size;
    return true;
}

static inline b8 s_file_write(const char* _path, const char* _data, sz _size) {
    if (_path == NULL || _path[0] == '\0') return false;
    if (_size > 0 && _data == NULL) return false;
    FILE* f = fopen(_path, "wb");
    if (f == NULL) return false;
    if (_size > 0 && fwrite(_data, 1, _size, f) != _size) { fclose(f); return false; }
    fclose(f);
    return true;
}

static inline b8 s_file_write_binary(const char* _path, const void* _data, sz _size) {
    if (_path == NULL || _path[0] == '\0') return false;
    if (_size > 0 && _data == NULL) return false;
    FILE* f = fopen(_path, "wb");
    if (f == NULL) return false;
    if (_size > 0 && fwrite(_data, 1, _size, f) != _size) { fclose(f); return false; }
    fclose(f);
    return true;
}

static inline b8 s_file_append(const char* _path, const char* _data, sz _size) {
    if (_path == NULL || _path[0] == '\0') return false;
    if (_size > 0 && _data == NULL) return false;
    FILE* f = fopen(_path, "ab");
    if (f == NULL) return false;
    if (_size > 0 && fwrite(_data, 1, _size, f) != _size) { fclose(f); return false; }
    fclose(f);
    return true;
}

static inline b8 s_file_remove(const char* _path) {
    if (_path == NULL || _path[0] == '\0') return false;
    return remove(_path) == 0;
}

static inline b8 s_file_copy(const char* _src, const char* _dst, b8 _overwrite) {
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
    if (ferror(fsrc)) { fclose(fsrc); fclose(fdst); return false; }
    fclose(fsrc);
    fclose(fdst);
    return true;
}

static inline b8 s_file_move(const char* _src, const char* _dst, b8 _overwrite) {
    if (_src == NULL || _dst == NULL || _src[0] == '\0' || _dst[0] == '\0') return false;
    if (!_overwrite && s_path_exists(_dst)) return false;
    if (_overwrite && s_path_exists(_dst)) {
        if (!s_file_remove(_dst)) return false;
    }
    if (rename(_src, _dst) == 0) return true;
    if (!s_file_copy(_src, _dst, true)) return false;
    return s_file_remove(_src);
}

static inline b8 s_file_size(const char* _path, sz* _out_size) {
    if (_path == NULL || _path[0] == '\0' || _out_size == NULL) return false;
    s_stat_t st;
    if (s_stat(_path, &st) != 0) return false;
    *_out_size = (sz)st.st_size;
    return true;
}

static inline b8 s_file_mtime(const char* _path, time_t* _out_mtime) {
    if (_path == NULL || _path[0] == '\0' || _out_mtime == NULL) return false;
    s_stat_t st;
    if (s_stat(_path, &st) != 0) return false;
    *_out_mtime = st.st_mtime;
    return true;
}

#endif // S_FILES_H
