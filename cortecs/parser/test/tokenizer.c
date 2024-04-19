#include <unity.h>
#include <tokenizer.h>
#include <string.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_tokenize_int(void) {
    char *text = "1";
    cortecs_tokenizer_result_t result = cortecs_tokenizer_next(text, 0);
    TEST_ASSERT_EQUAL_INT32(result.start, 1);
    TEST_ASSERT_TRUE(result.token.tag == CORTECS_TOKEN_INT);
    TEST_ASSERT_FALSE(strncmp(text, result.token.text, strlen(text)));
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_tokenize_int);
    return UNITY_END();
}