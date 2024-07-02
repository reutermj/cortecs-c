#include <flecs.h>
#include <gc.h>
#include <stdio.h>
#include <string.h>
#include <unity.h>
#include <world.h>

void test_remove_one_unconnected(void) {
    cortecs_world_init();
    cortecs_gc_init();

    cortecs_gc_allocation_t unreachable = cortecs_gc_alloc(128);

    ecs_progress(world, 0);

    TEST_ASSERT_FALSE(ecs_is_alive(world, unreachable.entity));

    cortecs_world_cleanup();
}

void test_remove_one_reachable_not_rooted(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t unroot_reachable = cortecs_gc_alloc(128);
    cortecs_gc_add(target, unroot_reachable);

    ecs_progress(world, 0);

    TEST_ASSERT_FALSE(ecs_is_alive(world, unroot_reachable.entity));

    cortecs_world_cleanup();
}

void test_keep_one_rooted(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t rooted = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));

    cortecs_world_cleanup();
}

void test_keep_one_rooted_one_root_reachable(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t rooted = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted);

    cortecs_gc_allocation_t root_reachable = cortecs_gc_alloc(128);
    cortecs_gc_add(rooted.entity, root_reachable);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, root_reachable.entity));

    cortecs_world_cleanup();
}

void test_remove_root_one_rooted(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t rooted = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));

    cortecs_gc_remove_root(target, rooted);

    ecs_progress(world, 0);

    TEST_ASSERT_FALSE(ecs_is_alive(world, rooted.entity));

    cortecs_world_cleanup();
}

void test_remove_root_one_rooted_one_kept(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t rooted_remove = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted_remove);

    cortecs_gc_allocation_t rooted_keep = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted_keep);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted_remove.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted_keep.entity));

    cortecs_gc_remove_root(target, rooted_remove);

    ecs_progress(world, 0);

    TEST_ASSERT_FALSE(ecs_is_alive(world, rooted_remove.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted_keep.entity));

    cortecs_world_cleanup();
}

void test_remove_root_one_rooted_one_root_reachable(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t rooted = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted);
    cortecs_gc_allocation_t root_reachable = cortecs_gc_alloc(128);
    cortecs_gc_add(rooted.entity, root_reachable);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, root_reachable.entity));

    cortecs_gc_remove_root(target, rooted);

    ecs_progress(world, 0);

    TEST_ASSERT_FALSE(ecs_is_alive(world, rooted.entity));
    TEST_ASSERT_FALSE(ecs_is_alive(world, root_reachable.entity));

    cortecs_world_cleanup();
}

void test_remove_reachable_keep_one_rooted_one_root_reachable_remove_one_root_reachable(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t rooted = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted);
    cortecs_gc_allocation_t root_reachable_keep = cortecs_gc_alloc(128);
    cortecs_gc_add(rooted.entity, root_reachable_keep);
    cortecs_gc_allocation_t root_reachable_remove = cortecs_gc_alloc(128);
    cortecs_gc_add(root_reachable_keep.entity, root_reachable_remove);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, root_reachable_keep.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, root_reachable_remove.entity));

    cortecs_gc_remove(root_reachable_keep.entity, root_reachable_remove);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, root_reachable_keep.entity));
    TEST_ASSERT_FALSE(ecs_is_alive(world, root_reachable_remove.entity));

    cortecs_world_cleanup();
}

void test_remove_reachable_keep_one_rooted_remove_two_root_reachable(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t rooted = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted);
    cortecs_gc_allocation_t root_reachable_remove1 = cortecs_gc_alloc(128);
    cortecs_gc_add(rooted.entity, root_reachable_remove1);
    cortecs_gc_allocation_t root_reachable_remove2 = cortecs_gc_alloc(128);
    cortecs_gc_add(root_reachable_remove1.entity, root_reachable_remove2);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, root_reachable_remove1.entity));
    TEST_ASSERT_TRUE(ecs_is_alive(world, root_reachable_remove2.entity));

    cortecs_gc_remove(rooted.entity, root_reachable_remove1);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));
    TEST_ASSERT_FALSE(ecs_is_alive(world, root_reachable_remove1.entity));
    TEST_ASSERT_FALSE(ecs_is_alive(world, root_reachable_remove2.entity));

    cortecs_world_cleanup();
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_remove_one_unconnected);
    RUN_TEST(test_remove_one_reachable_not_rooted);
    RUN_TEST(test_keep_one_rooted);
    RUN_TEST(test_keep_one_rooted_one_root_reachable);
    RUN_TEST(test_remove_root_one_rooted);
    RUN_TEST(test_remove_root_one_rooted_one_kept);
    RUN_TEST(test_remove_root_one_rooted_one_root_reachable);
    RUN_TEST(test_remove_reachable_keep_one_rooted_one_root_reachable_remove_one_root_reachable);
    RUN_TEST(test_remove_reachable_keep_one_rooted_remove_two_root_reachable);
    return UNITY_END();
}

void setUp() {
    // required for unity
}

void tearDown() {
    // required for unity
}