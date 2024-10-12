#include <cortecs/gc.h>
#include <cortecs/stdlib/hashmap.h>
#include <cortecs/world.h>
#include <unity.h>

#define TYPE_PARAM_KEY uint32_t
#define TYPE_PARAM_VALUE uint32_t

#include <cortecs/stdlib/hashmap.template.h>

#include <cortecs/stdlib/hashmap.template.c>  //NOLINT(bugprone-suspicious-include)

void test_new() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
    ecs_defer_begin(world);

    cortecs_hashmap_register_finalizer(uint32_t, uint32_t)();

    cortecs_hashmap(uint32_t, uint32_t) map = cortecs_hashmap_new(uint32_t, uint32_t)();
    TEST_ASSERT_NOT_NULL(map);
    TEST_ASSERT_EQUAL_INT(CORTECS_HASHMAP_NONE, map->tag);

    ecs_defer_end(world);
    cortecs_world_cleanup();
}

void test_set_and_get_one_value() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
    ecs_defer_begin(world);

    cortecs_hashmap_register_finalizer(uint32_t, uint32_t)();

    cortecs_hashmap(uint32_t, uint32_t) empty = cortecs_hashmap_new(uint32_t, uint32_t)();
    cortecs_hashmap(uint32_t, uint32_t) has_value = cortecs_hashmap_set(uint32_t, uint32_t)(empty, 10, 20);
    uint32_t retrieved_value = cortecs_hashmap_get(uint32_t, uint32_t)(has_value, 10);
    TEST_ASSERT_EQUAL_UINT32(20, retrieved_value);

    ecs_defer_end(world);
    cortecs_world_cleanup();
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_new);
    RUN_TEST(test_set_and_get_one_value);
    return UNITY_END();
}

void setUp() {
}

void tearDown() {
}