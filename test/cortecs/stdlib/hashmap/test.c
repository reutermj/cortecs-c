#include <cortecs/gc.h>
#include <cortecs/stdlib/hashmap.h>
#include <cortecs/world.h>
#include <unity.h>

#define TYPE_PARAM_KEY uint32_t
#define TYPE_PARAM_VALUE uint32_t

#include <cortecs/stdlib/hashmap.template.h>

#include <cortecs/stdlib/hashmap.template.c>  //NOLINT(bugprone-suspicious-include)

void test_new() {
    ecs_defer_begin(world);
    cortecs_hashmap(uint32_t, uint32_t) map = cortecs_hashmap_new(uint32_t, uint32_t)();
    TEST_ASSERT_NOT_NULL(map);
    TEST_ASSERT_EQUAL_INT(CORTECS_HASHMAP_NONE, map->tag);
    ecs_defer_end(world);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_new);
    return UNITY_END();
}

void setUp() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
}

void tearDown() {
    cortecs_world_cleanup();
}