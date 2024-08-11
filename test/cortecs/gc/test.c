#include <flecs.h>
#include <gc.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unity.h>
#include <world.h>

static void test_collect_unused_allocation() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(128, 0);
    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_used_allocation() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(128, 0);
    cortecs_gc_inc(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_TRUE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_then_collect() {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(128, 0);
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
        cortecs_gc_alloc(size, 0);
        ecs_defer_end(world);
    }
    cortecs_world_cleanup();
}

static bool noop_finalizer_called = false;
static void noop_finalizer() {
    noop_finalizer_called = true;
}

static void test_noop_finalizer() {
    cortecs_world_init();
    cortecs_gc_init();

    const uint32_t allocation_size = 128;
    cortecs_gc_type_index index = cortecs_gc_register_type(noop_finalizer, allocation_size);

    ecs_defer_begin(world);
    cortecs_gc_alloc(allocation_size, index);
    ecs_defer_end(world);

    TEST_ASSERT_TRUE(noop_finalizer_called);

    cortecs_world_cleanup();
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_collect_unused_allocation);
    RUN_TEST(test_keep_used_allocation);
    RUN_TEST(test_keep_then_collect);
    RUN_TEST(test_allocate_sizes);
    RUN_TEST(test_noop_finalizer);
    return UNITY_END();
}

void setUp() {
    srand(time(NULL));
}

void tearDown() {
    // required for unity
}