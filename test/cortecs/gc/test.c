#include <common.h>
#include <cortecs/array.h>
#include <cortecs/finalizer.h>
#include <cortecs/gc.h>
#include <cortecs/log.h>
#include <cortecs/world.h>
#include <flecs.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unity.h>

typedef struct {
    uint32_t the_data[5];
} some_data;
cortecs_finalizer_declare(some_data);
cortecs_array_declare(some_data);

static void test_collect_unused_allocation() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(some_data);
    TEST_ASSERT_NOT_NULL(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_collect_unused_allocation_array() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    ecs_defer_begin(world);
    const int size_of_array = 4;
    cortecs_array(void) allocation = cortecs_gc_alloc_array(some_data, size_of_array);
    TEST_ASSERT_NOT_NULL(allocation);

    TEST_ASSERT_EQUAL_UINT32(size_of_array, allocation->size);
    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_used_allocation() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(some_data);
    cortecs_gc_inc(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_TRUE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_used_allocation_array() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    ecs_defer_begin(world);
    cortecs_array(void) allocation = cortecs_gc_alloc_array(some_data, 4);
    cortecs_gc_inc(allocation);
    ecs_defer_end(world);

    TEST_ASSERT_TRUE(cortecs_gc_is_alive(allocation));

    cortecs_world_cleanup();
}

static void test_keep_then_collect() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    ecs_defer_begin(world);
    void *allocation = cortecs_gc_alloc(some_data);
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
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    ecs_defer_begin(world);
    cortecs_array(void) allocation = cortecs_gc_alloc_array(some_data, 4);
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
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
    for (uint32_t size = 32; size < 1024; size += 32) {
        ecs_defer_begin(world);
        cortecs_gc_alloc_impl(
            size,
            CORTECS_FINALIZER_NONE,
            __FILE__,
            __func__,
            __LINE__
        );
        ecs_defer_end(world);
    }
    cortecs_world_cleanup();
}

static void test_allocate_sizes_array() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
    for (uint32_t size_of_elements = 32; size_of_elements < 512; size_of_elements += 32) {
        for (uint32_t size_of_array = 1; size_of_array < 128; size_of_array++) {
            ecs_defer_begin(world);
            cortecs_array(void) array = cortecs_gc_alloc_array_impl(
                size_of_elements,
                size_of_array,
                8,
                CORTECS_FINALIZER_NONE,
                __FILE__,
                __func__,
                __LINE__
            );
            TEST_ASSERT_EQUAL_UINT32(size_of_array, array->size);
            ecs_defer_end(world);
        }
    }
    cortecs_world_cleanup();
}

typedef struct {
    uint32_t some_data[5];
} noop_data;
cortecs_finalizer_declare(noop_data);
cortecs_array_declare(noop_data);

static uint32_t noop_finalizer_called = 0;
void cortecs_finalizer(noop_data)(void *allocation) {
    UNUSED(allocation);
    noop_finalizer_called += 1;
}

static void test_noop_finalizer() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    cortecs_finalizer_register(noop_data);

    noop_finalizer_called = 0;
    ecs_defer_begin(world);
    cortecs_gc_alloc(noop_data);
    ecs_defer_end(world);

    TEST_ASSERT_EQUAL_UINT32(1, noop_finalizer_called);

    cortecs_world_cleanup();
}

static void test_noop_finalizer_array() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    cortecs_finalizer_register(noop_data);

    for (uint32_t size_of_array = 1; size_of_array < 128; size_of_array++) {
        noop_finalizer_called = 0;
        ecs_defer_begin(world);
        cortecs_gc_alloc_array(noop_data, size_of_array);
        ecs_defer_end(world);

        TEST_ASSERT_EQUAL_UINT32(size_of_array, noop_finalizer_called);
    }

    cortecs_world_cleanup();
}

typedef struct {
    void *target;
} single_target;
cortecs_array_declare(single_target);
cortecs_finalizer_define(single_target);

void cortecs_finalizer(single_target)(void *allocation) {
    single_target *data = allocation;
    if (data->target) {
        cortecs_gc_dec(data->target);
    }
}

static void test_1_recursive_collect() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    cortecs_finalizer_register(single_target);

    ecs_defer_begin(world);

    single_target *data = cortecs_gc_alloc(single_target);
    some_data *target = cortecs_gc_alloc(some_data);
    cortecs_gc_inc(target);
    data->target = target;

    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(data));
    TEST_ASSERT_FALSE(cortecs_gc_is_alive(target));

    cortecs_world_cleanup();
}

static void test_1_recursive_collect_array() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    cortecs_finalizer_register(single_target);

    some_data *targets[512];

    ecs_defer_begin(world);

    cortecs_array(single_target) data = cortecs_gc_alloc_array(single_target, 512);
    for (int i = 0; i < 512; i++) {
        targets[i] = cortecs_gc_alloc(some_data);
        cortecs_gc_inc(targets[i]);
        data->elements[i].target = targets[i];
    }

    ecs_defer_end(world);

    TEST_ASSERT_FALSE(cortecs_gc_is_alive(data));
    for (int i = 0; i < 512; i++) {
        TEST_ASSERT_FALSE(cortecs_gc_is_alive(targets[i]));
    }

    cortecs_world_cleanup();
}

static void test_n_recursive_collect() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    cortecs_finalizer_register(single_target);

    ecs_defer_begin(world);
    single_target *targets[512];

    targets[0] = cortecs_gc_alloc(single_target);
    targets[0]->target = NULL;
    for (int i = 1; i < 512; i++) {
        targets[i] = cortecs_gc_alloc(single_target);
        cortecs_gc_inc(targets[i - 1]);
        targets[i]->target = targets[i - 1];
    }

    ecs_defer_end(world);

    for (int i = 1; i < 512; i++) {
        if (cortecs_gc_is_alive(targets[i])) {
            NOOP;
        }
        TEST_ASSERT_FALSE(cortecs_gc_is_alive(targets[i]));
    }

    cortecs_world_cleanup();
}

static void test_gc_log_open_close() {
    const char *log_path = "./test_gc_log_open_close.log";
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_log_init();
    cortecs_gc_init(log_path);
    cortecs_gc_cleanup();
    cortecs_world_cleanup();

    FILE *log = fopen(log_path, "r");
    char line[2048];
    while (fgets(line, sizeof(line), log)) {
        printf("%s", line);
    }
    fclose(log);

    remove(log_path);
}

static void test_inc_dec_null() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);

    cortecs_gc_inc(NULL);
    cortecs_gc_dec(NULL);

    cortecs_world_cleanup();
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_gc_log_open_close);
    RUN_TEST(test_inc_dec_null);
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
    RUN_TEST(test_1_recursive_collect);
    RUN_TEST(test_1_recursive_collect_array);
    RUN_TEST(test_n_recursive_collect);

    return UNITY_END();
}

void setUp() {
    srand(time(NULL));
}

void tearDown() {
    // required for unity
}