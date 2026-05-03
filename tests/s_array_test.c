/**
 * Syphax - Ougi Washi
 *
 * Comprehensive tests for s_array.h (handle-based sparse array)
 *
 * The public macros are C99-compatible; some tests call base helpers directly
 * when they need explicit casts.
 */

#include "../s_array.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int g_tests_run = 0;
static int g_tests_passed = 0;

#define TEST(name) do { \
    g_tests_run++; \
    printf("  [TEST] %s... ", name); \
} while(0)

#define PASS() do { \
    g_tests_passed++; \
    printf("PASS\n"); \
} while(0)

#define ASSERT(cond, msg) do { \
    if (!(cond)) { \
        fprintf(stderr, "ASSERTION FAILED: %s\n", msg); \
        exit(1); \
    } \
} while(0)

#define ASSERT_EQ(a, b, msg) do { \
    if ((a) != (b)) { \
        fprintf(stderr, "ASSERTION FAILED: %s (expected %lld, got %lld)\n", msg, (long long)(b), (long long)(a)); \
        exit(1); \
    } \
} while(0)

/* Helper to get typed pointer from handle in C99 */
#define GET_INT(arr, handle) ((int*)s_array_get_ptr(&(arr)->b, sizeof(int), (handle)))
#define GET_DATA_INT(arr) ((int*)(arr)->b.data)

/* Test basic initialization */
static void test_init(void) {
    TEST("s_array_init");

    s_array(int, arr);
    s_array_init(&arr);

    ASSERT_EQ(s_array_get_size(&arr), 0, "initial size should be 0");
    ASSERT_EQ(s_array_get_capacity(&arr), 0, "initial capacity should be 0");

    s_array_clear(&arr);
    PASS();
}

/* Test basic add and get */
static void test_add_get(void) {
    TEST("s_array_add and s_array_get");

    s_array(int, arr);
    s_array_init(&arr);

    int val1 = 42;
    int val2 = 100;
    s_handle h1 = s_array_add(&arr, val1);
    s_handle h2 = s_array_add(&arr, val2);

    ASSERT_EQ(s_array_get_size(&arr), 2, "size after 2 adds");

    int* p1 = GET_INT(&arr, h1);
    int* p2 = GET_INT(&arr, h2);

    ASSERT(p1 != NULL, "p1 should not be NULL");
    ASSERT(p2 != NULL, "p2 should not be NULL");
    ASSERT_EQ(*p1, 42, "first value should be 42");
    ASSERT_EQ(*p2, 100, "second value should be 100");

    s_array_clear(&arr);
    PASS();
}

/* Test appending several values at once */
static void test_append_many(void) {
    TEST("s_array_append_many");

    s_array(int, arr);
    s_array_init(&arr);

    int values[] = {10, 20, 30};
    s_handle first = s_array_append_many(&arr, values, 3);

    ASSERT(first != S_HANDLE_NULL, "append_many should return first appended handle");
    ASSERT_EQ(s_array_get_size(&arr), 3, "size after append_many");
    ASSERT_EQ(*GET_INT(&arr, first), 10, "first appended handle should read first value");

    for (int i = 0; i < 3; ++i) {
        s_handle h = s_array_handle(&arr, (u32)i);
        ASSERT_EQ(*GET_INT(&arr, h), values[i], "dense value should match appended input");
    }

    s_handle empty = s_array_append_many(&arr, values, 0);
    ASSERT(empty == S_HANDLE_NULL, "zero append should return null handle");
    ASSERT_EQ(s_array_get_size(&arr), 3, "zero append should keep size");

    s_handle stale = s_array_handle(&arr, 1);
    ASSERT(s_array_remove(&arr, stale) == 1, "remove before free-slot append should succeed");
    ASSERT(GET_INT(&arr, stale) == NULL, "removed handle should be stale");

    int more[] = {40, 50};
    s_handle more_first = s_array_append_many(&arr, more, 2);

    ASSERT_EQ(s_array_get_size(&arr), 4, "size after appending into free slot");
    ASSERT_EQ(*GET_INT(&arr, more_first), 40, "first handle after free-slot append");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 0)), 10, "dense[0] unchanged");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 1)), 30, "dense[1] contains swap-removed value");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 2)), 40, "dense[2] first new value");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 3)), 50, "dense[3] second new value");

    s_array_clear(&arr);
    PASS();
}

/* Test s_array_increment */
static void test_increment(void) {
    TEST("s_array_increment");

    s_array(int, arr);
    s_array_init(&arr);

    s_handle h = s_array_increment(&arr);
    int* p = GET_INT(&arr, h);
    ASSERT(p != NULL, "increment should return valid pointer");
    ASSERT_EQ(*p, 0, "increment should zero-initialize");

    *p = 999;
    ASSERT_EQ(*GET_INT(&arr, h), 999, "value should be settable");

    s_array_clear(&arr);
    PASS();
}

/* Test s_array_handle */
static void test_handle(void) {
    TEST("s_array_handle");

    s_array(int, arr);
    s_array_init(&arr);

    int val1 = 10;
    int val2 = 20;
    int val3 = 30;
    s_array_add(&arr, val1);
    s_array_add(&arr, val2);
    s_array_add(&arr, val3);

    s_handle h0 = s_array_handle(&arr, 0);
    s_handle h1 = s_array_handle(&arr, 1);
    s_handle h2 = s_array_handle(&arr, 2);

    ASSERT_EQ(*GET_INT(&arr, h0), 10, "handle[0] should get value 10");
    ASSERT_EQ(*GET_INT(&arr, h1), 20, "handle[1] should get value 20");
    ASSERT_EQ(*GET_INT(&arr, h2), 30, "handle[2] should get value 30");

    s_array_clear(&arr);
    PASS();
}

/* Test handle slot/gen functions */
static void test_handle_decomposition(void) {
    TEST("s_handle_slot and s_handle_gen");

    s_handle h = s_handle_make(123, 456);
    ASSERT_EQ(s_handle_slot(h), 123, "slot should be 123");
    ASSERT_EQ(s_handle_gen(h), 456, "gen should be 456");

    PASS();
}

/* Test S_HANDLE_NULL */
static void test_null_handle(void) {
    TEST("S_HANDLE_NULL");

    s_array(int, arr);
    s_array_init(&arr);

    int* p = GET_INT(&arr, S_HANDLE_NULL);
    ASSERT(p == NULL, "S_HANDLE_NULL should return NULL");

    s_array_clear(&arr);
    PASS();
}

/* Test invalid/stale handles */
static void test_invalid_handles(void) {
    TEST("invalid handle returns NULL");

    s_array(int, arr);
    s_array_init(&arr);

    /* Create a handle, remove the element, try to use stale handle */
    int val = 42;
    s_handle h = s_array_add(&arr, val);
    s_array_remove(&arr, h);

    int* p = GET_INT(&arr, h);
    ASSERT(p == NULL, "stale handle should return NULL");

    /* Out of bounds dense index */
    s_handle bad_dense = s_array_handle(&arr, 999);
    ASSERT(bad_dense == S_HANDLE_NULL, "out of bounds dense should return NULL handle");

    s_array_clear(&arr);
    PASS();
}

/* Test s_array_remove (swap-remove) */
static void test_remove_fast(void) {
    TEST("s_array_remove (swap-remove)");

    s_array(int, arr);
    s_array_init(&arr);

    int v0 = 0, v1 = 1, v2 = 2, v3 = 3;
    s_array_add(&arr, v0);
    s_array_add(&arr, v1);
    s_array_add(&arr, v2);
    s_array_add(&arr, v3);

    ASSERT_EQ(s_array_get_size(&arr), 4, "size before remove");

    /* Remove element at dense index 1 (value 1) */
    s_handle h1 = s_array_handle(&arr, 1);
    b8 result = s_array_remove(&arr, h1);
    ASSERT(result == 1, "remove should return true");
    ASSERT_EQ(s_array_get_size(&arr), 3, "size after remove");

    /* With swap-remove, last element (3) should be at position 1 */
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 1)), 3, "swap-remove puts last at removed position");

    /* Stale handle should not work */
    ASSERT(GET_INT(&arr, h1) == NULL, "stale handle should return NULL");

    s_array_clear(&arr);
    PASS();
}

/* Test s_array_remove_ordered */
static void test_remove_ordered(void) {
    TEST("s_array_remove_ordered");

    s_array(int, arr);
    s_array_init(&arr);

    int v0 = 0, v1 = 1, v2 = 2, v3 = 3;
    s_array_add(&arr, v0);
    s_array_add(&arr, v1);
    s_array_add(&arr, v2);
    s_array_add(&arr, v3);

    /* Remove element at dense index 1 (value 1) */
    s_handle h1 = s_array_handle(&arr, 1);
    b8 result = s_array_remove_ordered(&arr, h1);
    ASSERT(result == 1, "remove_ordered should return true");
    ASSERT_EQ(s_array_get_size(&arr), 3, "size after remove");

    /* With ordered remove, elements should shift */
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 0)), 0, "first element unchanged");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 1)), 2, "second element shifted from 2");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 2)), 3, "third element shifted from 3");

    s_array_clear(&arr);
    PASS();
}

/* Test slot reuse with generation */
static void test_slot_reuse(void) {
    TEST("slot reuse with generation");

    s_array(int, arr);
    s_array_init(&arr);

    int val = 42;
    s_handle h1 = s_array_add(&arr, val);
    u32 gen1 = s_handle_gen(h1);

    s_array_remove(&arr, h1);

    /* Add another element - should reuse the slot */
    s_handle h2 = s_array_add(&arr, val);

    /* h2 should have same slot but different (incremented) generation */
    ASSERT_EQ(s_handle_slot(h1), s_handle_slot(h2), "should reuse same slot");
    ASSERT(s_handle_gen(h2) != gen1, "generation should be different");

    /* h1 should be stale now */
    ASSERT(GET_INT(&arr, h1) == NULL, "old handle should be stale");
    ASSERT(GET_INT(&arr, h2) != NULL, "new handle should work");

    s_array_clear(&arr);
    PASS();
}

/* Test with struct types */
typedef struct {
    int x;
    int y;
    char name[32];
} point_t;

#define GET_POINT(arr, handle) ((point_t*)s_array_get_ptr(&(arr)->b, sizeof(point_t), (handle)))

static void test_struct_type(void) {
    TEST("struct type operations");

    s_array(point_t, points);
    s_array_init(&points);

    point_t p1 = { .x = 10, .y = 20 };
    strncpy(p1.name, "point1", sizeof(p1.name) - 1);
    p1.name[sizeof(p1.name) - 1] = '\0';

    point_t p2 = { .x = 30, .y = 40 };
    strncpy(p2.name, "point2", sizeof(p2.name) - 1);
    p2.name[sizeof(p2.name) - 1] = '\0';

    s_handle h1 = s_array_add(&points, p1);
    s_handle h2 = s_array_add(&points, p2);

    point_t* rp1 = GET_POINT(&points, h1);
    point_t* rp2 = GET_POINT(&points, h2);

    ASSERT(rp1 != NULL && rp1->x == 10 && rp1->y == 20, "p1 values correct");
    ASSERT(rp2 != NULL && rp2->x == 30 && rp2->y == 40, "p2 values correct");
    ASSERT(strcmp(rp1->name, "point1") == 0, "p1 name correct");
    ASSERT(strcmp(rp2->name, "point2") == 0, "p2 name correct");

    /* Modify through pointer */
    rp1->x = 100;
    ASSERT(GET_POINT(&points, h1)->x == 100, "modification through pointer works");

    s_array_clear(&points);
    PASS();
}

/* Test s_array_get_data (using raw data access) */
static void test_get_data(void) {
    TEST("s_array_get_data");

    s_array(int, arr);
    s_array_init(&arr);

    int v0 = 10, v1 = 20, v2 = 30;
    s_array_add(&arr, v0);
    s_array_add(&arr, v1);
    s_array_add(&arr, v2);

    /* Get contiguous data pointer */
    int* data = GET_DATA_INT(&arr);
    ASSERT(data != NULL, "data pointer should not be NULL");

    /* Note: data is packed, so indices 0,1,2 correspond to dense order */
    ASSERT_EQ(data[0], 10, "data[0] should be 10");
    ASSERT_EQ(data[1], 20, "data[1] should be 20");
    ASSERT_EQ(data[2], 30, "data[2] should be 30");

    s_array_clear(&arr);
    PASS();
}

/* Test capacity growth */
static void test_capacity_growth(void) {
    TEST("capacity growth");

    s_array(int, arr);
    s_array_init(&arr);

    sz initial_cap = s_array_get_capacity(&arr);
    ASSERT_EQ(initial_cap, 0, "initial capacity is 0");

    /* Add many elements to trigger growth */
    for (int i = 0; i < 100; i++) {
        s_array_add(&arr, i);
    }

    ASSERT_EQ(s_array_get_size(&arr), 100, "size is 100");
    ASSERT(s_array_get_capacity(&arr) >= 100, "capacity should be >= 100");

    /* Verify all values */
    for (int i = 0; i < 100; i++) {
        s_handle h = s_array_handle(&arr, (u32)i);
        int* p = GET_INT(&arr, h);
        ASSERT(p != NULL && *p == i, "value should match");
    }

    s_array_clear(&arr);
    PASS();
}

/* Test explicit reserve API */
static void test_reserve_api(void) {
    TEST("s_array_reserve API");

    s_array(int, arr);
    s_array_init(&arr);

    int v0 = 10, v1 = 20, v2 = 30, v3 = 40;
    s_handle h0 = s_array_add(&arr, v0);
    s_handle h1 = s_array_add(&arr, v1);
    s_handle h2 = s_array_add(&arr, v2);
    s_handle h3 = s_array_add(&arr, v3);

    /* Create a stale handle before reserve */
    ASSERT(s_array_remove(&arr, h1) == 1, "remove should succeed");
    ASSERT(GET_INT(&arr, h1) == NULL, "removed handle should be stale before reserve");

    /* Rebuild known packed order to verify reserve does not reorder */
    ASSERT(s_array_remove_ordered(&arr, h0) == 1, "ordered remove h0");
    ASSERT(s_array_remove_ordered(&arr, h2) == 1, "ordered remove h2");
    ASSERT_EQ(*GET_INT(&arr, h3), 40, "h3 remains after ordered removes");
    h0 = s_array_add(&arr, v0);
    h2 = s_array_add(&arr, v2);

    s_array_reserve(&arr, 128);
    ASSERT(s_array_get_capacity(&arr) >= 128, "reserve should grow capacity");
    ASSERT_EQ(s_array_get_size(&arr), 3, "reserve should not change size");

    /* Existing valid handles and packed order must remain valid */
    ASSERT(GET_INT(&arr, h1) == NULL, "stale handle remains stale after reserve");
    ASSERT_EQ(*GET_INT(&arr, h3), 40, "h3 remains valid after reserve");
    ASSERT_EQ(*GET_INT(&arr, h0), 10, "h0 remains valid after reserve");
    ASSERT_EQ(*GET_INT(&arr, h2), 30, "h2 remains valid after reserve");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 0)), 40, "dense[0] order preserved");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 1)), 10, "dense[1] order preserved");
    ASSERT_EQ(*GET_INT(&arr, s_array_handle(&arr, 2)), 30, "dense[2] order preserved");

    s_array_clear(&arr);
    PASS();
}

/* Test remove from middle and end */
static void test_edge_cases(void) {
    TEST("edge cases - remove first/last/single");

    s_array(int, arr);
    s_array_init(&arr);

    /* Single element */
    int val = 42;
    s_handle h = s_array_add(&arr, val);
    s_array_remove(&arr, h);
    ASSERT_EQ(s_array_get_size(&arr), 0, "size after removing single element");

    /* Remove first of many */
    for (int i = 0; i < 5; i++) {
        s_array_add(&arr, i);
    }
    s_handle h0 = s_array_handle(&arr, 0);
    s_array_remove(&arr, h0);
    ASSERT_EQ(s_array_get_size(&arr), 4, "size after remove first");

    s_array_clear(&arr);

    /* Remove last */
    for (int i = 0; i < 5; i++) {
        s_array_add(&arr, i);
    }
    s_handle h4 = s_array_handle(&arr, 4);
    s_array_remove(&arr, h4);
    ASSERT_EQ(s_array_get_size(&arr), 4, "size after remove last");

    s_array_clear(&arr);
    PASS();
}

/* Test double clear */
static void test_double_clear(void) {
    TEST("double clear is safe");

    s_array(int, arr);
    s_array_init(&arr);

    int v1 = 1, v2 = 2;
    s_array_add(&arr, v1);
    s_array_add(&arr, v2);

    s_array_clear(&arr);
    s_array_clear(&arr);  /* Should not crash */

    ASSERT_EQ(s_array_get_size(&arr), 0, "size after double clear");

    PASS();
}

/* Test iteration pattern */
static void test_iteration(void) {
    TEST("iteration pattern");

    s_array(int, arr);
    s_array_init(&arr);

    for (int i = 0; i < 10; i++) {
        int val = i * 10;
        s_array_add(&arr, val);
    }

    int sum = 0;
    for (sz i = 0; i < s_array_get_size(&arr); ++i) {
        s_handle h = s_array_handle(&arr, (u32)i);
        int* p = GET_INT(&arr, h);
        ASSERT(p != NULL, "all handles should be valid during iteration");
        sum += *p;
    }

    ASSERT_EQ(sum, 450, "sum should be 0+10+20+...+90 = 450");

    s_array_clear(&arr);
    PASS();
}

/* Test generation wrap-around protection */
static void test_generation_wrap(void) {
    TEST("generation never 0 (wrap protection)");

    s_array(int, arr);
    s_array_init(&arr);

    /* Add and remove same slot many times */
    for (int i = 0; i < 10; i++) {
        int val = i;
        s_handle h = s_array_add(&arr, val);
        u32 gen = s_handle_gen(h);
        ASSERT(gen != 0, "generation should never be 0");
        s_array_remove(&arr, h);
    }

    /* S_HANDLE_NULL should still be 0 */
    ASSERT(S_HANDLE_NULL == 0, "S_HANDLE_NULL is 0");

    s_array_clear(&arr);
    PASS();
}

/* Test out of bounds slot access */
static void test_bounds_checking(void) {
    TEST("bounds checking");

    s_array(int, arr);
    s_array_init(&arr);
    int value = 1;
    s_handle h = s_array_add(&arr, value);
    ASSERT(h != S_HANDLE_NULL, "setup add should work");

    /* Create a handle with slot beyond array bounds */
    s_handle bad_handle = s_handle_make(9999, 1);
    int* p = GET_INT(&arr, bad_handle);
    ASSERT(p == NULL, "out of bounds slot should return NULL");
#if SIZE_MAX > UINT32_MAX
    ASSERT(s_array_handle(&arr, (sz)UINT32_MAX + 1u) == S_HANDLE_NULL, "large dense index should not wrap");
#endif

    s_array_clear(&arr);
    PASS();
}

/* Test removing with wrong generation */
static void test_wrong_generation(void) {
    TEST("wrong generation");

    s_array(int, arr);
    s_array_init(&arr);

    int val = 42;
    s_handle h = s_array_add(&arr, val);
    u32 slot = s_handle_slot(h);
    u32 wrong_gen = s_handle_gen(h) + 1;

    s_handle bad_handle = s_handle_make(slot, wrong_gen);
    ASSERT(GET_INT(&arr, bad_handle) == NULL, "wrong gen should return NULL");

    s_array_clear(&arr);
    PASS();
}

/* Test add after clear */
static void test_add_after_clear(void) {
    TEST("add after clear");

    s_array(int, arr);
    s_array_init(&arr);

    int v1 = 1, v2 = 2;
    s_array_add(&arr, v1);
    s_array_add(&arr, v2);
    s_array_clear(&arr);

    int v100 = 100;
    s_handle h = s_array_add(&arr, v100);
    ASSERT_EQ(s_array_get_size(&arr), 1, "size after re-add");
    ASSERT_EQ(*GET_INT(&arr, h), 100, "value after re-add");

    s_array_clear(&arr);
    PASS();
}

/* Test s_array_require doesn't trigger on valid use */
static void test_no_false_asserts(void) {
    TEST("no false assertions");

    s_array(int, arr);
    s_array_init(&arr);

    /* All these should work without assertions */
    for (int i = 0; i < 1000; i++) {
        int val = i;
        s_array_add(&arr, val);
    }

    for (sz i = 0; i < s_array_get_size(&arr); i += 2) {
        s_handle h = s_array_handle(&arr, (u32)i);
        s_array_remove(&arr, h);
    }

    /* Add more to trigger slot reuse */
    for (int i = 0; i < 500; i++) {
        int val = i;
        s_array_add(&arr, val);
    }

    s_array_clear(&arr);
    PASS();
}

/* Test many removes and adds (stress test) */
static void test_stress(void) {
    TEST("stress test - many operations");

    s_array(int, arr);
    s_array_init(&arr);

    #define STRESS_N 10000
    s_handle handles[STRESS_N];

    /* Add many */
    for (int i = 0; i < STRESS_N; i++) {
        handles[i] = s_array_add(&arr, i);
    }

    ASSERT_EQ(s_array_get_size(&arr), STRESS_N, "size after bulk add");

    /* Remove every other */
    for (int i = 0; i < STRESS_N; i += 2) {
        s_array_remove(&arr, handles[i]);
    }

    ASSERT_EQ(s_array_get_size(&arr), STRESS_N / 2, "size after removing half");

    /* Verify odd indices still work */
    for (int i = 1; i < STRESS_N; i += 2) {
        int* p = GET_INT(&arr, handles[i]);
        ASSERT(p != NULL && *p == i, "odd indices should still be valid");
    }

    /* Even indices should be stale */
    for (int i = 0; i < STRESS_N; i += 2) {
        ASSERT(GET_INT(&arr, handles[i]) == NULL, "even indices should be stale");
    }

    s_array_clear(&arr);
    PASS();
}

/* Test ordered remove preserves correctness */
static void test_ordered_remove_correctness(void) {
    TEST("ordered remove correctness");

    s_array(int, arr);
    s_array_init(&arr);

    /* Add 0..9 */
    for (int i = 0; i < 10; i++) {
        s_array_add(&arr, i);
    }

    /* Remove middle element (value 5 at dense index 5) */
    s_handle h5 = s_array_handle(&arr, 5);
    s_array_remove_ordered(&arr, h5);

    /* Dense array should now be 0,1,2,3,4,6,7,8,9 */
    int expected[] = {0, 1, 2, 3, 4, 6, 7, 8, 9};
    for (int i = 0; i < 9; i++) {
        s_handle h = s_array_handle(&arr, (u32)i);
        int* p = GET_INT(&arr, h);
        ASSERT(p != NULL && *p == expected[i], "ordered remove should shift elements");
    }

    s_array_clear(&arr);
    PASS();
}

/* Test empty array operations */
static void test_empty_array(void) {
    TEST("empty array operations");

    s_array(int, arr);
    s_array_init(&arr);

    /* These should all be safe on empty array */
    ASSERT_EQ(s_array_get_size(&arr), 0, "empty size");
    ASSERT_EQ(s_array_get_capacity(&arr), 0, "empty capacity");
    ASSERT(arr.b.data == NULL, "empty data is NULL");

    /* Iteration should do nothing */
    int count = 0;
    for (sz i = 0; i < s_array_get_size(&arr); i++) {
        count++;
    }
    ASSERT_EQ(count, 0, "empty iteration does nothing");

    s_array_clear(&arr);
    PASS();
}

int main(void) {
    printf("=== s_array comprehensive test suite ===\n\n");

    test_init();
    test_add_get();
    test_append_many();
    test_increment();
    test_handle();
    test_handle_decomposition();
    test_null_handle();
    test_invalid_handles();
    test_remove_fast();
    test_remove_ordered();
    test_slot_reuse();
    test_struct_type();
    test_get_data();
    test_capacity_growth();
    test_reserve_api();
    test_edge_cases();
    test_double_clear();
    test_iteration();
    test_generation_wrap();
    test_bounds_checking();
    test_wrong_generation();
    test_add_after_clear();
    test_no_false_asserts();
    test_stress();
    test_ordered_remove_correctness();
    test_empty_array();

    printf("\n=== Results ===\n");
    printf("Tests run: %d\n", g_tests_run);
    printf("Tests passed: %d\n", g_tests_passed);

    if (g_tests_passed == g_tests_run) {
        printf("\nAll s_array tests PASSED!\n");
        return 0;
    } else {
        printf("\nSome tests FAILED!\n");
        return 1;
    }
}
