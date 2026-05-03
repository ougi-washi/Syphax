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
typedef struct s_thread_win_state s_thread_win_state;
typedef struct { HANDLE handle; s_thread_id id; s_thread_win_state* state; b8 joined; b8 detached; } s_thread;
typedef struct { CRITICAL_SECTION cs; } s_mutex;
typedef struct { CONDITION_VARIABLE cv; } s_cond;
#else
typedef pthread_t s_thread_id;
typedef struct { pthread_t handle; b8 joined; b8 detached; } s_thread;
typedef struct { pthread_mutex_t mtx; } s_mutex;
typedef struct { pthread_cond_t cv; } s_cond;
#endif

#if !defined(SYPHAX_STATIC)
s_thread_id s_thread_current_id(void);
b8 s_thread_id_equal(s_thread_id _a, s_thread_id _b);
b8 s_thread_create(s_thread* _thread, s_thread_fn _fn, void* _arg);
b8 s_thread_join(s_thread* _thread, void** _out_ret);
b8 s_thread_detach(s_thread* _thread);
void s_thread_sleep_ms(u32 _ms);
void s_thread_yield(void);
b8 s_mutex_init(s_mutex* _mtx);
b8 s_mutex_destroy(s_mutex* _mtx);
b8 s_mutex_lock(s_mutex* _mtx);
b8 s_mutex_try_lock(s_mutex* _mtx);
b8 s_mutex_unlock(s_mutex* _mtx);
b8 s_cond_init(s_cond* _cond);
b8 s_cond_destroy(s_cond* _cond);
b8 s_cond_wait(s_cond* _cond, s_mutex* _mtx);
b8 s_cond_wait_ms(s_cond* _cond, s_mutex* _mtx, u32 _timeout_ms);
b8 s_cond_signal(s_cond* _cond);
b8 s_cond_broadcast(s_cond* _cond);
#endif

#if defined(SYPHAX_STATIC) || defined(SYPHAX_IMPLEMENTATION)
#if defined(SYPHAX_STATIC)
#define S_THREAD_DEF static inline
#else
#define S_THREAD_DEF
#endif

#if defined(_WIN32)
struct s_thread_win_state {
    s_thread_fn fn;
    void* arg;
    void* ret;
    volatile LONG lifecycle;
};

static DWORD WINAPI s_thread_start_trampoline(void* _arg) {
    s_thread_win_state* state = (s_thread_win_state*)_arg;
    state->ret = state->fn(state->arg);
    if (InterlockedCompareExchange(&state->lifecycle, 2, 0) == 1) {
        s_free(state);
    }
    return 0;
}
#endif

S_THREAD_DEF b8 s_thread_create(s_thread* _thread, s_thread_fn _fn, void* _arg) {
    if (_thread == NULL || _fn == NULL) return false;
    *_thread = (s_thread){0};
#if defined(_WIN32)
    s_thread_win_state* state = (s_thread_win_state*)s_malloc(sizeof(s_thread_win_state));
    if (state == NULL) return false;
    state->fn = _fn;
    state->arg = _arg;
    state->ret = NULL;
    state->lifecycle = 0;
    _thread->state = state;
    _thread->handle = CreateThread(NULL, 0, s_thread_start_trampoline, state, 0, &_thread->id);
    if (_thread->handle == NULL) {
        s_free(state);
        _thread->state = NULL;
        return false;
    }
    return true;
#else
    return pthread_create(&_thread->handle, NULL, _fn, _arg) == 0;
#endif
}

S_THREAD_DEF b8 s_thread_join(s_thread* _thread, void** _out_ret) {
    if (_thread == NULL || _thread->joined || _thread->detached) return false;
#if defined(_WIN32)
    if (_thread->handle == NULL) return false;
    s_thread_win_state* state = _thread->state;
    DWORD wait_result = WaitForSingleObject(_thread->handle, INFINITE);
    if (wait_result != WAIT_OBJECT_0) return false;
    if (_out_ret != NULL) *_out_ret = state != NULL ? state->ret : NULL;
    CloseHandle(_thread->handle);
    s_free(state);
    _thread->handle = NULL;
    _thread->id = 0;
    _thread->state = NULL;
    _thread->joined = true;
    return true;
#else
    void* ret = NULL;
    if (pthread_join(_thread->handle, _out_ret ? _out_ret : &ret) != 0) return false;
    _thread->joined = true;
    return true;
#endif
}

S_THREAD_DEF b8 s_thread_detach(s_thread* _thread) {
    if (_thread == NULL || _thread->joined || _thread->detached) return false;
#if defined(_WIN32)
    if (_thread->handle == NULL) return false;
    s_thread_win_state* state = _thread->state;
    CloseHandle(_thread->handle);
    _thread->handle = NULL;
    _thread->id = 0;
    _thread->state = NULL;
    _thread->detached = true;
    if (state != NULL && InterlockedCompareExchange(&state->lifecycle, 1, 0) == 2) {
        s_free(state);
    }
    return true;
#else
    if (pthread_detach(_thread->handle) != 0) return false;
    _thread->detached = true;
    return true;
#endif
}

S_THREAD_DEF void s_thread_sleep_ms(u32 _ms) {
#if defined(_WIN32)
    Sleep(_ms);
#else
    struct timespec ts;
    ts.tv_sec = _ms / 1000u;
    ts.tv_nsec = (long)(_ms % 1000u) * 1000000L;
    nanosleep(&ts, NULL);
#endif
}

S_THREAD_DEF void s_thread_yield(void) {
#if defined(_WIN32)
    SwitchToThread();
#else
    sched_yield();
#endif
}

S_THREAD_DEF s_thread_id s_thread_current_id(void) {
#if defined(_WIN32)
    return GetCurrentThreadId();
#else
    return pthread_self();
#endif
}

S_THREAD_DEF b8 s_thread_id_equal(s_thread_id _a, s_thread_id _b) {
#if defined(_WIN32)
    return _a == _b;
#else
    return pthread_equal(_a, _b) != 0;
#endif
}

S_THREAD_DEF b8 s_mutex_init(s_mutex* _mtx) {
    if (_mtx == NULL) return false;
#if defined(_WIN32)
    InitializeCriticalSection(&_mtx->cs);
    return true;
#else
    return pthread_mutex_init(&_mtx->mtx, NULL) == 0;
#endif
}

S_THREAD_DEF b8 s_mutex_destroy(s_mutex* _mtx) {
    if (_mtx == NULL) return false;
#if defined(_WIN32)
    DeleteCriticalSection(&_mtx->cs);
    return true;
#else
    return pthread_mutex_destroy(&_mtx->mtx) == 0;
#endif
}

S_THREAD_DEF b8 s_mutex_lock(s_mutex* _mtx) {
    if (_mtx == NULL) return false;
#if defined(_WIN32)
    EnterCriticalSection(&_mtx->cs);
    return true;
#else
    return pthread_mutex_lock(&_mtx->mtx) == 0;
#endif
}

S_THREAD_DEF b8 s_mutex_try_lock(s_mutex* _mtx) {
    if (_mtx == NULL) return false;
#if defined(_WIN32)
    return TryEnterCriticalSection(&_mtx->cs) != 0;
#else
    return pthread_mutex_trylock(&_mtx->mtx) == 0;
#endif
}

S_THREAD_DEF b8 s_mutex_unlock(s_mutex* _mtx) {
    if (_mtx == NULL) return false;
#if defined(_WIN32)
    LeaveCriticalSection(&_mtx->cs);
    return true;
#else
    return pthread_mutex_unlock(&_mtx->mtx) == 0;
#endif
}

S_THREAD_DEF b8 s_cond_init(s_cond* _cond) {
    if (_cond == NULL) return false;
#if defined(_WIN32)
    InitializeConditionVariable(&_cond->cv);
    return true;
#else
    return pthread_cond_init(&_cond->cv, NULL) == 0;
#endif
}

S_THREAD_DEF b8 s_cond_destroy(s_cond* _cond) {
    if (_cond == NULL) return false;
#if defined(_WIN32)
    (void)_cond;
    return true;
#else
    return pthread_cond_destroy(&_cond->cv) == 0;
#endif
}

S_THREAD_DEF b8 s_cond_wait(s_cond* _cond, s_mutex* _mtx) {
    if (_cond == NULL || _mtx == NULL) return false;
#if defined(_WIN32)
    return SleepConditionVariableCS(&_cond->cv, &_mtx->cs, INFINITE) != 0;
#else
    return pthread_cond_wait(&_cond->cv, &_mtx->mtx) == 0;
#endif
}

S_THREAD_DEF b8 s_cond_wait_ms(s_cond* _cond, s_mutex* _mtx, u32 _timeout_ms) {
    if (_cond == NULL || _mtx == NULL) return false;
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

S_THREAD_DEF b8 s_cond_signal(s_cond* _cond) {
    if (_cond == NULL) return false;
#if defined(_WIN32)
    WakeConditionVariable(&_cond->cv);
    return true;
#else
    return pthread_cond_signal(&_cond->cv) == 0;
#endif
}

S_THREAD_DEF b8 s_cond_broadcast(s_cond* _cond) {
    if (_cond == NULL) return false;
#if defined(_WIN32)
    WakeAllConditionVariable(&_cond->cv);
    return true;
#else
    return pthread_cond_broadcast(&_cond->cv) == 0;
#endif
}

#undef S_THREAD_DEF
#endif

#endif // S_THREAD_H
