#include <cortecs/gc.h>
#include <cortecs/string.h>
#include <cortecs/world.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

static void run_test_new_string(const char *target) {
    uint32_t target_length = strlen(target);
    cortecs_string out = cortecs_string_new("%s", target);
    TEST_ASSERT_EQUAL_UINT32(target_length, out->size);
    TEST_ASSERT_EQUAL_MEMORY(target, out->content, target_length + 1);  // + 1 for the null terminator
}

static void test_copy_cstring(void) {
    run_test_new_string("");
    run_test_new_string("hello world");
}

static void test_equality(const char *left, const char *right, bool areEqual) {
    cortecs_string left_str = cortecs_string_new("%s", left);
    cortecs_string right_str = cortecs_string_new("%s", right);

    TEST_ASSERT_TRUE(cortecs_string_equals(left_str, right_str) == areEqual);
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
    cortecs_type_init();
    cortecs_gc_init(NULL);
    ecs_defer_begin(world);
}

void tearDown() {
    ecs_defer_end(world);
    cortecs_world_cleanup();
}