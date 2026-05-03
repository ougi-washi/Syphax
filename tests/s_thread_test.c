/*
 * Simple thread and sync tests for Syphax.
 */

#define SYPHAX_STATIC
#include "../s_thread.h"
#include <stdio.h>

typedef struct {
    s_mutex mtx;
    s_cond cond;
    s_mutex try_mtx;
    i32 ready;
    i32 saw_ready;
    i32 count;
    i32 try_lock_ok;
    i32 sync_done;
} s_shared;

typedef struct {
    s_shared* shared;
    i32 iterations;
} s_worker_args;

static void* s_worker(void* _arg) {
    s_worker_args* args = (s_worker_args*)_arg;
    for (i32 i = 0; i < args->iterations; i++) {
        s_mutex_lock(&args->shared->mtx);
        args->shared->count += 1;
        s_mutex_unlock(&args->shared->mtx);
    }
    return NULL;
}

static void* s_waiter(void* _arg) {
    s_shared* shared = (s_shared*)_arg;
    s_mutex_lock(&shared->mtx);
    while (!shared->ready) {
        s_cond_wait(&shared->cond, &shared->mtx);
    }
    shared->saw_ready = 1;
    s_mutex_unlock(&shared->mtx);
    return NULL;
}

static void* s_set_ready(void* _arg) {
    s_shared* shared = (s_shared*)_arg;
    s_thread_sleep_ms(50);
    s_mutex_lock(&shared->mtx);
    shared->ready = 1;
    s_cond_signal(&shared->cond);
    s_mutex_unlock(&shared->mtx);
    return NULL;
}

static void* s_detached_worker(void* _arg) {
    s_shared* shared = (s_shared*)_arg;
    for (i32 i = 0; i < 1000; i++) {
        s_mutex_lock(&shared->mtx);
        shared->count += 1;
        s_mutex_unlock(&shared->mtx);
        s_thread_yield();
    }
    return NULL;
}

static void* s_try_lock_worker(void* _arg) {
    s_shared* shared = (s_shared*)_arg;
    i32 ok = 0;
    if (s_mutex_try_lock(&shared->try_mtx)) {
        ok = 1;
        s_mutex_unlock(&shared->try_mtx);
    }
    s_mutex_lock(&shared->mtx);
    shared->try_lock_ok = ok;
    shared->sync_done = 1;
    s_cond_signal(&shared->cond);
    s_mutex_unlock(&shared->mtx);
    return NULL;
}

int main(void) {
    s_shared shared = {0};
    s_assertf(s_mutex_init(&shared.mtx), "s_mutex_init failed\n");
    s_assertf(s_cond_init(&shared.cond), "s_cond_init failed\n");
    s_assertf(s_mutex_init(&shared.try_mtx), "s_mutex_init(try_mtx) failed\n");

    const i32 thread_count = 4;
    const i32 iterations = 25000;
    s_thread threads[thread_count];
    s_worker_args args = {0};
    args.shared = &shared;
    args.iterations = iterations;

    for (i32 i = 0; i < thread_count; i++) {
        s_assertf(s_thread_create(&threads[i], s_worker, &args), "s_thread_create failed\n");
    }

    for (i32 i = 0; i < thread_count; i++) {
        s_assertf(s_thread_join(&threads[i], NULL), "s_thread_join failed\n");
    }
    s_assertf(!s_thread_join(&threads[0], NULL), "double join should fail\n");

    const i32 expected = thread_count * iterations;
    s_assertf(shared.count == expected, "shared.count mismatch\n");

    s_thread waiter;
    s_assertf(s_thread_create(&waiter, s_waiter, &shared), "s_thread_create(waiter) failed\n");
    s_thread setter;
    s_assertf(s_thread_create(&setter, s_set_ready, &shared), "s_thread_create(setter) failed\n");

    s_assertf(s_thread_join(&waiter, NULL), "s_thread_join(waiter) failed\n");
    s_assertf(s_thread_join(&setter, NULL), "s_thread_join(setter) failed\n");
    s_assertf(shared.saw_ready == 1, "saw_ready not set\n");

    s_mutex_lock(&shared.mtx);
    s_assertf(!s_cond_wait_ms(&shared.cond, &shared.mtx, 1), "s_cond_wait_ms should time out\n");
    s_mutex_unlock(&shared.mtx);

    s_thread detached;
    s_assertf(s_thread_create(&detached, s_detached_worker, &shared), "s_thread_create(detached) failed\n");
    s_assertf(s_thread_detach(&detached), "s_thread_detach failed\n");
    s_assertf(!s_thread_join(&detached, NULL), "join after detach should fail\n");
    s_assertf(!s_thread_detach(&detached), "double detach should fail\n");
    s_thread_sleep_ms(50);

    s_thread try_lock_thread;
    shared.sync_done = 0;
    s_mutex_lock(&shared.try_mtx);
    s_assertf(s_thread_create(&try_lock_thread, s_try_lock_worker, &shared), "s_thread_create(try_lock) failed\n");
    s_mutex_lock(&shared.mtx);
    while (!shared.sync_done) {
        s_cond_wait(&shared.cond, &shared.mtx);
    }
    s_mutex_unlock(&shared.mtx);
    s_mutex_unlock(&shared.try_mtx);
    s_assertf(s_thread_join(&try_lock_thread, NULL), "s_thread_join(try_lock) failed\n");
    s_assertf(shared.try_lock_ok == 0, "s_mutex_try_lock should fail when locked\n");
    s_assertf(s_mutex_try_lock(&shared.try_mtx), "s_mutex_try_lock failed\n");
    s_mutex_unlock(&shared.try_mtx);

    s_thread_id current = s_thread_current_id();
    s_assertf(s_thread_id_equal(current, s_thread_current_id()), "s_thread_id_equal failed\n");
    s_assertf(!s_thread_create(NULL, s_worker, &args), "s_thread_create NULL thread should fail\n");
    s_assertf(!s_thread_create(&threads[0], NULL, &args), "s_thread_create NULL fn should fail\n");
    s_assertf(!s_thread_join(NULL, NULL), "s_thread_join NULL should fail\n");
    s_assertf(!s_thread_detach(NULL), "s_thread_detach NULL should fail\n");
    s_assertf(!s_mutex_init(NULL), "s_mutex_init NULL should fail\n");
    s_assertf(!s_mutex_lock(NULL), "s_mutex_lock NULL should fail\n");
    s_assertf(!s_mutex_unlock(NULL), "s_mutex_unlock NULL should fail\n");
    s_assertf(!s_mutex_try_lock(NULL), "s_mutex_try_lock NULL should fail\n");
    s_assertf(!s_mutex_destroy(NULL), "s_mutex_destroy NULL should fail\n");
    s_assertf(!s_cond_init(NULL), "s_cond_init NULL should fail\n");
    s_assertf(!s_cond_wait(NULL, &shared.mtx), "s_cond_wait NULL cond should fail\n");
    s_assertf(!s_cond_wait(&shared.cond, NULL), "s_cond_wait NULL mutex should fail\n");
    s_assertf(!s_cond_wait_ms(NULL, &shared.mtx, 1), "s_cond_wait_ms NULL cond should fail\n");
    s_assertf(!s_cond_signal(NULL), "s_cond_signal NULL should fail\n");
    s_assertf(!s_cond_broadcast(NULL), "s_cond_broadcast NULL should fail\n");
    s_assertf(!s_cond_destroy(NULL), "s_cond_destroy NULL should fail\n");

    s_cond_destroy(&shared.cond);
    s_mutex_destroy(&shared.mtx);
    s_mutex_destroy(&shared.try_mtx);
    printf("s_thread tests passed\n");
    return 0;
}
