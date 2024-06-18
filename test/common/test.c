#include <persistent_string.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unity.h>

static string_t load_string(const char *text) {
    uint32_t length = strlen(text);
    uint8_t *u8_text = malloc(sizeof(uint8_t) * (length + 1));
    memcpy(u8_text, text, length);
    u8_text[length] = 0;

    return (string_t){
        .content = u8_text,
        .length = length,
    };
}

static void test_equality(const char *left, const char *right, bool areEqual) {
    string_t left_str = load_string(left);
    string_t right_str = load_string(right);

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