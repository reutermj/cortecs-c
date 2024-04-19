#include <unity.h>
#include <tokenizer.h>
#include <string.h>
#include <stdint.h>

void setUp(void) {
    // set stuff up here
}

void tearDown(void) {
    // clean stuff up here
}

void test_tokenize(char *in, uint32_t offset, char *gold, cortecs_token_tag_t tag) {
    cortecs_tokenizer_result_t result = cortecs_tokenizer_next(in, offset);
    int target_length = strlen(gold);
    TEST_ASSERT_EQUAL_INT32(offset + target_length, result.start);
    TEST_ASSERT_TRUE(result.token.tag == tag);
    TEST_ASSERT_TRUE(strncmp(gold, result.token.text, strlen(gold)) == 0);
}

void test_tokenize_int(void) {
    test_tokenize("1", 0, "1", CORTECS_TOKEN_INT);
    test_tokenize("123", 0, "123", CORTECS_TOKEN_INT);
    test_tokenize("123 asdf", 0, "123", CORTECS_TOKEN_INT);
    test_tokenize("asdf 123", 5, "123", CORTECS_TOKEN_INT);
    test_tokenize("asdf 123 qwer", 5, "123", CORTECS_TOKEN_INT);
}

void test_tokenize_function(void) {
    test_tokenize("function", 0, "function", CORTECS_TOKEN_FUNCTION);
    test_tokenize("asdf function", 5, "function", CORTECS_TOKEN_FUNCTION);
    test_tokenize("function 123", 0, "function", CORTECS_TOKEN_FUNCTION);
    test_tokenize("asdf function 123", 5, "function", CORTECS_TOKEN_FUNCTION);
}

void test_tokenize_let(void) {
    test_tokenize("let", 0, "let", CORTECS_TOKEN_LET);
    test_tokenize("asdf let", 5, "let", CORTECS_TOKEN_LET);
    test_tokenize("let 123", 0, "let", CORTECS_TOKEN_LET);
    test_tokenize("asdf let 123", 5, "let", CORTECS_TOKEN_LET);
}

void test_tokenize_return(void) {
    test_tokenize("return", 0, "return", CORTECS_TOKEN_RETURN);
    test_tokenize("asdf return", 5, "return", CORTECS_TOKEN_RETURN);
    test_tokenize("return 123", 0, "return", CORTECS_TOKEN_RETURN);
    test_tokenize("asdf return 123", 5, "return", CORTECS_TOKEN_RETURN);
}

void test_tokenize_if(void) {
    test_tokenize("if", 0, "if", CORTECS_TOKEN_IF);
    test_tokenize("asdf if", 5, "if", CORTECS_TOKEN_IF);
    test_tokenize("if 123", 0, "if", CORTECS_TOKEN_IF);
    test_tokenize("asdf if 123", 5, "if", CORTECS_TOKEN_IF);
}

void test_tokenize_name(void) {
    test_tokenize("a", 0, "a", CORTECS_TOKEN_NAME);
    test_tokenize("asdf", 0, "asdf", CORTECS_TOKEN_NAME);
    test_tokenize("a123", 0, "a123", CORTECS_TOKEN_NAME);
    test_tokenize("a123_b456", 0, "a123_b456", CORTECS_TOKEN_NAME);
    test_tokenize("123 asdf", 4, "asdf", CORTECS_TOKEN_NAME);
    test_tokenize("asdf 123", 0, "asdf", CORTECS_TOKEN_NAME);
    test_tokenize("qwer asdf 123", 5, "asdf", CORTECS_TOKEN_NAME);
}

void test_tokenize_type(void) {
    test_tokenize("A", 0, "A", CORTECS_TOKEN_TYPE);
    test_tokenize("Asdf", 0, "Asdf", CORTECS_TOKEN_TYPE);
    test_tokenize("A123", 0, "A123", CORTECS_TOKEN_TYPE);
    test_tokenize("A123_b456", 0, "A123_b456", CORTECS_TOKEN_TYPE);
    test_tokenize("123 Asdf", 4, "Asdf", CORTECS_TOKEN_TYPE);
    test_tokenize("Asdf 123", 0, "Asdf", CORTECS_TOKEN_TYPE);
    test_tokenize("qwer Asdf 123", 5, "Asdf", CORTECS_TOKEN_TYPE);
}

// not needed when using generate_test_runner.rb
int main(void) {
    UNITY_BEGIN();
    RUN_TEST(test_tokenize_int);
    RUN_TEST(test_tokenize_function);
    RUN_TEST(test_tokenize_let);
    RUN_TEST(test_tokenize_return);
    RUN_TEST(test_tokenize_if);
    RUN_TEST(test_tokenize_name);
    RUN_TEST(test_tokenize_type);
    return UNITY_END();
}