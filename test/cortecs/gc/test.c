#include <common.h>
#include <cortecs/array.h>
#include <cortecs/gc.h>
#include <cortecs/world.h>
#include <flecs.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unity.h>

static void test_collect_unused_allocation() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(128, CORTECS_GC_NO_FINALIZER);
    TEST_ASSERT_NOT_NULL(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_collect_unused_allocation_array() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    cortecs_array(void) allocation = cortecs_gc_alloc_array(128, 4, CORTECS_GC_NO_FINALIZER);
    TEST_ASSERT_NOT_NULL(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_used_allocation() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(128, CORTECS_GC_NO_FINALIZER);
    cortecs_gc_inc(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_TRUE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_used_allocation_array() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    cortecs_array(void) allocation = cortecs_gc_alloc_array(128, 4, CORTECS_GC_NO_FINALIZER);
    cortecs_gc_inc(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_TRUE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_then_collect() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(128, CORTECS_GC_NO_FINALIZER);
    cortecs_gc_inc(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_TRUE(cortecs_gc_is_alive(allocation));

    ecs_defer_begin(world);
    cortecs_gc_dec(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_then_collect_array() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    cortecs_array(void) allocation = cortecs_gc_alloc_array(128, 4, CORTECS_GC_NO_FINALIZER);
    cortecs_gc_inc(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_TRUE(cortecs_gc_is_alive(allocation));

    ecs_defer_begin(world);
    cortecs_gc_dec(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_allocate_sizes() {
    cortecs_world_init();
    cortecs_gc_init();
    for (uint32_t size = 32; size < 1024; size += 32) {
        ecs_defer_begin(world);
        cortecs_gc_alloc(size, CORTECS_GC_NO_FINALIZER);
        ecs_defer_end(world);
    }
    cortecs_world_cleanup();
}

static void test_allocate_sizes_array() {
    cortecs_world_init();
    cortecs_gc_init();
    for (uint32_t size_of_elements = 32; size_of_elements < 512; size_of_elements += 32) {
        for (uint32_t size_of_array = 0; size_of_array < 128; size_of_array++) {
            ecs_defer_begin(world);
            cortecs_gc_alloc_array(size_of_elements, size_of_array, CORTECS_GC_NO_FINALIZER);
            ecs_defer_end(world);
        }
    }
    cortecs_world_cleanup();
}

static uint32_t noop_finalizer_called = 0;
static void noop_finalizer(void *allocation) {
    UNUSED(allocation);
    noop_finalizer_called += 1;
}

static void test_noop_finalizer() {
    cortecs_world_init();
    cortecs_gc_init();

    cortecs_gc_finalizer_index noop = cortecs_gc_register_finalizer(noop_finalizer, 128);

    noop_finalizer_called = 0;
    ecs_defer_begin(world);
    cortecs_gc_alloc(128, noop);
    ecs_defer_end(world);

    TEST_ASSERT_EQUAL_UINT32(1, noop_finalizer_called);

    cortecs_world_cleanup();
}

static void test_noop_finalizer_array() {
    cortecs_world_init();
    cortecs_gc_init();

    cortecs_gc_finalizer_index noop = cortecs_gc_register_finalizer(noop_finalizer, 128);

    for (uint32_t size_of_elements = 1; size_of_elements < 128; size_of_elements++) {
        noop_finalizer_called = 0;
        ecs_defer_begin(world);
        cortecs_gc_alloc_array(128, size_of_elements, noop);
        ecs_defer_end(world);

        TEST_ASSERT_EQUAL_UINT32(size_of_elements, noop_finalizer_called);
    }

    cortecs_world_cleanup();
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_collect_unused_allocation);
    RUN_TEST(test_collect_unused_allocation_array);
    RUN_TEST(test_keep_used_allocation);
    RUN_TEST(test_keep_used_allocation_array);
    RUN_TEST(test_keep_then_collect);
    RUN_TEST(test_keep_then_collect_array);
    RUN_TEST(test_allocate_sizes);
    RUN_TEST(test_allocate_sizes_array);
    RUN_TEST(test_noop_finalizer);
    RUN_TEST(test_noop_finalizer_array);

    return UNITY_END();
}

void setUp() {
    srand(time(NULL));
}

void tearDown() {
    // required for unity
}