#include <cortecs/gc.h>
#include <cortecs/string.h>
#include <cortecs/world.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

static void run_test_new_string(const char *target) {
    uint32_t target_length = strlen(target) + 1;
    CN(Cortecs, String) out = CN(Cortecs, String, new)("%s", target);
    TEST_ASSERT_EQUAL_UINT32(target_length, CN(Cortecs, String, capacity)(out));
    TEST_ASSERT_EQUAL_MEMORY(target, out.content->elements, target_length);
}

static void test_copy_cstring(void) {
    run_test_new_string("");
    run_test_new_string("hello world");
}

static void test_equality(const char *left, const char *right, bool areEqual) {
    CN(Cortecs, String) left_str = CN(Cortecs, String, new)("%s", left);
    CN(Cortecs, String) right_str = CN(Cortecs, String, new)("%s", right);

    TEST_ASSERT_TRUE(CN(Cortecs, String, equals)(left_str, right_str) == areEqual);
}

static void test_string_equals(void) {
    test_equality("", "", true);
    test_equality("foo", "foo", true);
    test_equality("", "foo", false);
    test_equality("foobar", "baz", false);
    test_equality("foo", "bar", false);
    test_equality("foo", "foobar", false);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_copy_cstring);
    RUN_TEST(test_string_equals);
    return UNITY_END();
}

void setUp() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
    ecs_defer_begin(world);
}

void tearDown() {
    ecs_defer_end(world);
    cortecs_world_cleanup();
}