#include <flecs.h>
#include <gc.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
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

void test_delete_entity_one_rooted(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ecs_entity_t target = ecs_new(world);
    cortecs_gc_allocation_t rooted = cortecs_gc_alloc(128);
    cortecs_gc_add_root(target, rooted);

    ecs_progress(world, 0);

    TEST_ASSERT_TRUE(ecs_is_alive(world, rooted.entity));

    ecs_delete(world, target);

    ecs_progress(world, 0);

    TEST_ASSERT_FALSE(ecs_is_alive(world, rooted.entity));

    cortecs_world_cleanup();
}

typedef struct {
    uint32_t *memory;
    uint32_t gold;
} pointer_stability;

void pointer_stability_system(ecs_iter_t *iterator) {
    pointer_stability *data = ecs_field(iterator, pointer_stability, 0);
    for (int i = 0; i < iterator->count; i++) {
        TEST_ASSERT_EQUAL_UINT32(data[i].gold, *data[i].memory);
    }
}

void test_pointer_stability(void) {
    cortecs_world_init();
    cortecs_gc_init();

    ECS_COMPONENT(world, pointer_stability);
    ECS_SYSTEM(world, pointer_stability_system, EcsOnUpdate, pointer_stability);

    uint32_t value = 0;
    const uint32_t num_entities = 10000;
    ecs_entity_t entities[num_entities];
    for (uint32_t i = 0; i < num_entities; i++) {
        entities[i] = ecs_new(world);
        cortecs_gc_allocation_t allocation = cortecs_gc_alloc(sizeof(uint32_t));
        cortecs_gc_add_root(entities[i], allocation);
        pointer_stability data = (pointer_stability){
            .memory = allocation.memory,
            .gold = value,
        };
        *data.memory = value;
        ecs_set_id(world, entities[i], ecs_id(pointer_stability), sizeof(pointer_stability), &data);
        value++;
    }

    ecs_progress(world, 0);
    bool marked[num_entities];
    memset(marked, false, sizeof(marked));
    for (uint32_t run = 0; run < 1000; run++) {
        for (uint32_t i = 0; i < num_entities; i++) {
            if (rand() % 2 == 1) {
                ecs_delete(world, entities[i]);
                marked[i] = true;
            }
        }

        ecs_progress(world, 0);

        for (uint32_t i = 0; i < num_entities; i++) {
            if (marked[i]) {
                entities[i] = ecs_new(world);
                cortecs_gc_allocation_t allocation = cortecs_gc_alloc(sizeof(uint32_t));
                cortecs_gc_add_root(entities[i], allocation);
                pointer_stability data = (pointer_stability){
                    .memory = allocation.memory,
                    .gold = value,
                };
                *data.memory = value;
                ecs_set_id(world, entities[i], ecs_id(pointer_stability), sizeof(pointer_stability), &data);
                value++;
                marked[i] = false;
            }
        }

        ecs_progress(world, 0);
    }

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
    RUN_TEST(test_delete_entity_one_rooted);
    // currently there's a flecs bug preventing this test from working
    // RUN_TEST(test_pointer_stability);
    return UNITY_END();
}

void setUp() {
    srand(time(NULL));
}

void tearDown() {
    // required for unity
}