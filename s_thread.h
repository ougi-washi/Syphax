/**
 * Syphax - Ougi Washi
 *
 * Easy to use threads and mutexes for C.
 * Cross-platform thread, mutex, and condition variable helpers.
 *
 * MIT License
 * Copyright (c) 2025-2026 Jed - Ougi Washi https://github.com/ougi-washi/Syphax
 */

#ifndef S_THREAD_H
#define S_THREAD_H

#include "s_types.h"
#include <stdlib.h>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#else
#include <pthread.h>
#include <time.h>
#include <sys/time.h>
#include <sched.h>
#include <unistd.h>
#endif

typedef void* (*s_thread_fn)(void*);

#if defined(_WIN32)
typedef DWORD s_thread_id;
typedef struct { HANDLE handle; s_thread_id id; } s_thread;
typedef struct { CRITICAL_SECTION cs; } s_mutex;
typedef struct { CONDITION_VARIABLE cv; } s_cond;
#else
typedef pthread_t s_thread_id;
typedef struct { pthread_t handle; } s_thread;
typedef struct { pthread_mutex_t mtx; } s_mutex;
typedef struct { pthread_cond_t cv; } s_cond;
#endif

static inline s_thread_id s_thread_current_id(void);
static inline b8 s_thread_id_equal(s_thread_id _a, s_thread_id _b);

#if defined(_WIN32)
typedef struct {
    s_thread_fn fn;
    void* arg;
} s_thread_start;

static DWORD WINAPI s_thread_start_trampoline(void* _arg) {
    s_thread_start* start = (s_thread_start*)_arg;
    s_thread_fn fn = start->fn;
    void* arg = start->arg;
    free(start);
    void* ret = fn(arg);
    return (DWORD)(uintptr_t)ret;
}
#endif

static inline b8 s_thread_create(s_thread* _thread, s_thread_fn _fn, void* _arg) {
    s_assertf(_thread != NULL, "s_thread_create :: Thread is null\n");
    s_assertf(_fn != NULL, "s_thread_create :: Function is null\n");
#if defined(_WIN32)
    s_thread_start* start = (s_thread_start*)malloc(sizeof(s_thread_start));
    if (start == NULL) return false;
    start->fn = _fn;
    start->arg = _arg;
    _thread->handle = CreateThread(NULL, 0, s_thread_start_trampoline, start, 0, &_thread->id);
    if (_thread->handle == NULL) {
        free(start);
        return false;
    }
    return true;
#else
    return pthread_create(&_thread->handle, NULL, _fn, _arg) == 0;
#endif
}

static inline b8 s_thread_join(s_thread* _thread, void** _out_ret) {
    s_assertf(_thread != NULL, "s_thread_join :: Thread is null\n");
#if defined(_WIN32)
    DWORD wait_result = WaitForSingleObject(_thread->handle, INFINITE);
    if (wait_result != WAIT_OBJECT_0) return false;
    if (_out_ret != NULL) {
        DWORD code = 0;
        if (!GetExitCodeThread(_thread->handle, &code)) return false;
        *_out_ret = (void*)(uintptr_t)code;
    }
    CloseHandle(_thread->handle);
    _thread->handle = NULL;
    _thread->id = 0;
    return true;
#else
    void* ret = NULL;
    if (pthread_join(_thread->handle, _out_ret ? _out_ret : &ret) != 0) return false;
    return true;
#endif
}

static inline b8 s_thread_detach(s_thread* _thread) {
    s_assertf(_thread != NULL, "s_thread_detach :: Thread is null\n");
#if defined(_WIN32)
    if (_thread->handle == NULL) return false;
    CloseHandle(_thread->handle);
    _thread->handle = NULL;
    _thread->id = 0;
    return true;
#else
    return pthread_detach(_thread->handle) == 0;
#endif
}

static inline void s_thread_sleep_ms(u32 _ms) {
#if defined(_WIN32)
    Sleep(_ms);
#else
    struct timespec ts;
    ts.tv_sec = _ms / 1000u;
    ts.tv_nsec = (long)(_ms % 1000u) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

static inline void s_thread_yield(void) {
#if defined(_WIN32)
    SwitchToThread();
#else
    sched_yield();
#endif
}

static inline s_thread_id s_thread_current_id(void) {
#if defined(_WIN32)
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

static inline b8 s_thread_id_equal(s_thread_id _a, s_thread_id _b) {
#if defined(_WIN32)
    return _a == _b;
#else
    return pthread_equal(_a, _b) != 0;
#endif
}

static inline b8 s_mutex_init(s_mutex* _mtx) {
    s_assertf(_mtx != NULL, "s_mutex_init :: Mutex is null\n");
#if defined(_WIN32)
    InitializeCriticalSection(&_mtx->cs);
    return true;
#else
    return pthread_mutex_init(&_mtx->mtx, NULL) == 0;
#endif
}

static inline void s_mutex_destroy(s_mutex* _mtx) {
    s_assertf(_mtx != NULL, "s_mutex_destroy :: Mutex is null\n");
#if defined(_WIN32)
    DeleteCriticalSection(&_mtx->cs);
#else
    pthread_mutex_destroy(&_mtx->mtx);
#endif
}

static inline void s_mutex_lock(s_mutex* _mtx) {
    s_assertf(_mtx != NULL, "s_mutex_lock :: Mutex is null\n");
#if defined(_WIN32)
    EnterCriticalSection(&_mtx->cs);
#else
    pthread_mutex_lock(&_mtx->mtx);
#endif
}

static inline b8 s_mutex_try_lock(s_mutex* _mtx) {
    s_assertf(_mtx != NULL, "s_mutex_try_lock :: Mutex is null\n");
#if defined(_WIN32)
    return TryEnterCriticalSection(&_mtx->cs) != 0;
#else
    return pthread_mutex_trylock(&_mtx->mtx) == 0;
#endif
}

static inline void s_mutex_unlock(s_mutex* _mtx) {
    s_assertf(_mtx != NULL, "s_mutex_unlock :: Mutex is null\n");
#if defined(_WIN32)
    LeaveCriticalSection(&_mtx->cs);
#else
    pthread_mutex_unlock(&_mtx->mtx);
#endif
}

static inline b8 s_cond_init(s_cond* _cond) {
    s_assertf(_cond != NULL, "s_cond_init :: Cond is null\n");
#if defined(_WIN32)
    InitializeConditionVariable(&_cond->cv);
    return true;
#else
    return pthread_cond_init(&_cond->cv, NULL) == 0;
#endif
}

static inline void s_cond_destroy(s_cond* _cond) {
    s_assertf(_cond != NULL, "s_cond_destroy :: Cond is null\n");
#if defined(_WIN32)
    (void)_cond;
#else
    pthread_cond_destroy(&_cond->cv);
#endif
}

static inline void s_cond_wait(s_cond* _cond, s_mutex* _mtx) {
    s_assertf(_cond != NULL, "s_cond_wait :: Cond is null\n");
    s_assertf(_mtx != NULL, "s_cond_wait :: Mutex is null\n");
#if defined(_WIN32)
    SleepConditionVariableCS(&_cond->cv, &_mtx->cs, INFINITE);
#else
    pthread_cond_wait(&_cond->cv, &_mtx->mtx);
#endif
}

static inline b8 s_cond_wait_ms(s_cond* _cond, s_mutex* _mtx, u32 _timeout_ms) {
    s_assertf(_cond != NULL, "s_cond_wait_ms :: Cond is null\n");
    s_assertf(_mtx != NULL, "s_cond_wait_ms :: Mutex is null\n");
#if defined(_WIN32)
    return SleepConditionVariableCS(&_cond->cv, &_mtx->cs, _timeout_ms) != 0;
#else
    struct timespec ts;
#if defined(CLOCK_REALTIME)
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000L;
#endif
    ts.tv_sec += _timeout_ms / 1000u;
    ts.tv_nsec += (long)(_timeout_ms % 1000u) * 1000000L;
    if (ts.tv_nsec >= 1000000000L) {
        ts.tv_sec += 1;
        ts.tv_nsec -= 1000000000L;
    }
    return pthread_cond_timedwait(&_cond->cv, &_mtx->mtx, &ts) == 0;
#endif
}

static inline void s_cond_signal(s_cond* _cond) {
    s_assertf(_cond != NULL, "s_cond_signal :: Cond is null\n");
#if defined(_WIN32)
    WakeConditionVariable(&_cond->cv);
#else
    pthread_cond_signal(&_cond->cv);
#endif
}

static inline void s_cond_broadcast(s_cond* _cond) {
    s_assertf(_cond != NULL, "s_cond_broadcast :: Cond is null\n");
#if defined(_WIN32)
    WakeAllConditionVariable(&_cond->cv);
#else
    pthread_cond_broadcast(&_cond->cv);
#endif
}

#endif // S_THREAD_H
