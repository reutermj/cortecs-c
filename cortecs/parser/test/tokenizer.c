#include <unity.h>
#include <tokenizer.h>
#include <string.h>
#include <stdint.h>

void setUp() {
    // set stuff up here
}

void tearDown() {
    // clean stuff up here
}

void test_tokenize(char *in, uint32_t offset, char *gold, cortecs_token_tag_t tag) {
    cortecs_tokenizer_result_t result = cortecs_tokenizer_next(in, offset);
    int target_length = strlen(gold);
    TEST_ASSERT_EQUAL_INT32(offset + target_length, result.start);
    TEST_ASSERT_TRUE(result.token.tag == tag);
    TEST_ASSERT_TRUE(strncmp(gold, result.token.text, strlen(gold)) == 0);
}

void test_tokenize_int() {
    test_tokenize("1", 0, "1", CORTECS_TOKEN_INT);
    test_tokenize("123", 0, "123", CORTECS_TOKEN_INT);
    test_tokenize("123 asdf", 0, "123", CORTECS_TOKEN_INT);
    test_tokenize("asdf 123", 5, "123", CORTECS_TOKEN_INT);
    test_tokenize("asdf 123 qwer", 5, "123", CORTECS_TOKEN_INT);
}

void test_tokenize_function() {
    test_tokenize("function", 0, "function", CORTECS_TOKEN_FUNCTION);
    test_tokenize("asdf function", 5, "function", CORTECS_TOKEN_FUNCTION);
    test_tokenize("function 123", 0, "function", CORTECS_TOKEN_FUNCTION);
    test_tokenize("asdf function 123", 5, "function", CORTECS_TOKEN_FUNCTION);
}

void test_tokenize_let() {
    test_tokenize("let", 0, "let", CORTECS_TOKEN_LET);
    test_tokenize("asdf let", 5, "let", CORTECS_TOKEN_LET);
    test_tokenize("let 123", 0, "let", CORTECS_TOKEN_LET);
    test_tokenize("asdf let 123", 5, "let", CORTECS_TOKEN_LET);
}

void test_tokenize_return() {
    test_tokenize("return", 0, "return", CORTECS_TOKEN_RETURN);
    test_tokenize("asdf return", 5, "return", CORTECS_TOKEN_RETURN);
    test_tokenize("return 123", 0, "return", CORTECS_TOKEN_RETURN);
    test_tokenize("asdf return 123", 5, "return", CORTECS_TOKEN_RETURN);
}

void test_tokenize_if() {
    test_tokenize("if", 0, "if", CORTECS_TOKEN_IF);
    test_tokenize("asdf if", 5, "if", CORTECS_TOKEN_IF);
    test_tokenize("if 123", 0, "if", CORTECS_TOKEN_IF);
    test_tokenize("asdf if 123", 5, "if", CORTECS_TOKEN_IF);
}

void test_tokenize_name() {
    test_tokenize("a", 0, "a", CORTECS_TOKEN_NAME);
    test_tokenize("asdf", 0, "asdf", CORTECS_TOKEN_NAME);
    test_tokenize("a123", 0, "a123", CORTECS_TOKEN_NAME);
    test_tokenize("a123_b456", 0, "a123_b456", CORTECS_TOKEN_NAME);
    test_tokenize("123 asdf", 4, "asdf", CORTECS_TOKEN_NAME);
    test_tokenize("asdf 123", 0, "asdf", CORTECS_TOKEN_NAME);
    test_tokenize("qwer asdf 123", 5, "asdf", CORTECS_TOKEN_NAME);
}

void test_tokenize_type() {
    test_tokenize("A", 0, "A", CORTECS_TOKEN_TYPE);
    test_tokenize("Asdf", 0, "Asdf", CORTECS_TOKEN_TYPE);
    test_tokenize("A123", 0, "A123", CORTECS_TOKEN_TYPE);
    test_tokenize("A123_b456", 0, "A123_b456", CORTECS_TOKEN_TYPE);
    test_tokenize("123 Asdf", 4, "Asdf", CORTECS_TOKEN_TYPE);
    test_tokenize("Asdf 123", 0, "Asdf", CORTECS_TOKEN_TYPE);
    test_tokenize("qwer Asdf 123", 5, "Asdf", CORTECS_TOKEN_TYPE);
}

void test_tokenize_whitespace() {
    test_tokenize(" ", 0, " ", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("\t", 0, "\t", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("  \t\t  ", 0, "  \t\t  ", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("  \t\t  123", 0, "  \t\t  ", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("asdf  \t", 4, "  \t", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("asdf  \t123", 4, "  \t", CORTECS_TOKEN_WHITESPACE);
}

void test_tokenize_newline() {
    test_tokenize("\n", 0, "\n", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("\n\n", 0, "\n", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("asdf\n", 4, "\n", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("\nasdf", 0, "\n", CORTECS_TOKEN_WHITESPACE);
    test_tokenize("asdf\nasdf", 4, "\n", CORTECS_TOKEN_WHITESPACE);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(test_tokenize_int);
    RUN_TEST(test_tokenize_function);
    RUN_TEST(test_tokenize_let);
    RUN_TEST(test_tokenize_return);
    RUN_TEST(test_tokenize_if);
    RUN_TEST(test_tokenize_name);
    RUN_TEST(test_tokenize_type);
    RUN_TEST(test_tokenize_whitespace);
    RUN_TEST(test_tokenize_newline);
    return UNITY_END();
}