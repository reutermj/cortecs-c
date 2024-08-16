#include <cortecs/string.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

static void run_test_copy_cstring(const char *target) {
    uint32_t target_length = strlen(target);
    string_t out = string_copy_cstring(target);
    TEST_ASSERT_EQUAL_UINT32(target_length, out.length);
    TEST_ASSERT_EQUAL_MEMORY(target, out.content, target_length + 1);  // + 1 for the null terminator
    string_cleanup(out);
}

static void test_copy_cstring(void) {
    run_test_copy_cstring("");
    run_test_copy_cstring("hello world");
}

static void test_equality(const char *left, const char *right, bool areEqual) {
    string_t left_str = string_copy_cstring(left);
    string_t right_str = string_copy_cstring(right);

    TEST_ASSERT_TRUE(string_equals(left_str, right_str) == areEqual);

    string_cleanup(left_str);
    string_cleanup(right_str);
}

static void test_string_equals(void) {
    test_equality("", "", true);
    test_equality("foo", "foo", true);
    test_equality("", "foo", false);
    test_equality("foobar", "baz", false);
    test_equality("foo", "bar", false);
    test_equality("foo", "foobar", false);
}

static void test_null_cleanup(void) {
    string_cleanup((string_t){.content = NULL});
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_copy_cstring);
    RUN_TEST(test_string_equals);
    RUN_TEST(test_null_cleanup);
    return UNITY_END();
}

void setUp() {
    // required for unity
}

void tearDown() {
    // required for unity
}