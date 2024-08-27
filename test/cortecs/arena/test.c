#include <common.h>
#include <cortecs/arena.h>
#include <cortecs/array.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unity.h>

static void test_collect_unused_allocation() {
    void *allocation = cortecs_arena_alloc(128);
    TEST_ASSERT_NOT_NULL(allocation);
    cortecs_arena_reset();
}

static void test_collect_unused_allocation_array() {
    cortecs_array(void) allocation = cortecs_arena_alloc_array(128, 4);
    TEST_ASSERT_NOT_NULL(allocation);
    cortecs_arena_reset();
}

static void test_resets_to_base() {
    cortecs_arena_reset();
    uint64_t base0 = (uint64_t)cortecs_arena_alloc(128);
    cortecs_arena_reset();
    uint64_t base1 = (uint64_t)cortecs_arena_alloc(128);
    TEST_ASSERT_EQUAL_UINT64(base0, base1);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_collect_unused_allocation);
    RUN_TEST(test_collect_unused_allocation_array);
    RUN_TEST(test_resets_to_base);

    return UNITY_END();
}

void setUp() {
    srand(time(NULL));
    cortecs_arena_init();
}

void tearDown() {
    // required for unity
}