#include "s_thread.h"
#include <stdio.h>

typedef struct {
    s_mutex mtx;
    s_cond cond;
    i32 count;
    i32 ready;
} s_shared;

typedef struct {
    s_shared* shared;
    i32 iterations;
} s_worker_args;

static void* s_worker(void* arg) {
    s_worker_args* args = (s_worker_args*)arg;
    for (i32 i = 0; i < args->iterations; i++) {
        s_mutex_lock(&args->shared->mtx);
        args->shared->count += 1;
        s_mutex_unlock(&args->shared->mtx);
    }
    return NULL;
}

static void* s_waiter(void* arg) {
    s_shared* shared = (s_shared*)arg;
    s_mutex_lock(&shared->mtx);
    while (!shared->ready) {
        s_cond_wait(&shared->cond, &shared->mtx);
    }
    s_mutex_unlock(&shared->mtx);
    return NULL;
}

static void* s_set_ready(void* arg) {
    s_shared* shared = (s_shared*)arg;
    s_thread_sleep_ms(20);
    s_mutex_lock(&shared->mtx);
    shared->ready = 1;
    s_cond_signal(&shared->cond);
    s_mutex_unlock(&shared->mtx);
    return NULL;
}

int main(void) {
    s_shared shared = {0};
    s_assertf(s_mutex_init(&shared.mtx), "s_mutex_init failed\n");
    s_assertf(s_cond_init(&shared.cond), "s_cond_init failed\n");

    enum { THREAD_COUNT = 4, ITERATIONS = 10000 };
    s_thread threads[THREAD_COUNT];
    s_worker_args args = {0};
    args.shared = &shared;
    args.iterations = ITERATIONS;

    for (i32 i = 0; i < THREAD_COUNT; i++) {
        s_assertf(s_thread_create(&threads[i], s_worker, &args), "s_thread_create failed\n");
    }

    for (i32 i = 0; i < THREAD_COUNT; i++) {
        s_assertf(s_thread_join(&threads[i], NULL), "s_thread_join failed\n");
    }

    const i32 expected = THREAD_COUNT * ITERATIONS;
    s_assertf(shared.count == expected, "count mismatch\n");
    printf("count=%d\n", shared.count);

    s_thread waiter;
    s_thread setter;
    s_assertf(s_thread_create(&waiter, s_waiter, &shared), "s_thread_create(waiter) failed\n");
    s_assertf(s_thread_create(&setter, s_set_ready, &shared), "s_thread_create(setter) failed\n");
    s_assertf(s_thread_join(&waiter, NULL), "s_thread_join(waiter) failed\n");
    s_assertf(s_thread_join(&setter, NULL), "s_thread_join(setter) failed\n");

    s_cond_destroy(&shared.cond);
    s_mutex_destroy(&shared.mtx);
    return 0;
}
