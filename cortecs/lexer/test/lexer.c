#include <lexer.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>
#include <unity.h>

#include "fuzz.h"

void setUp() {
    cortecs_lexer_fuzz_init();
}

void tearDown() {
    // required for unity
}

void cortecs_lexer_test(char *in, uint32_t offset, char *gold, cortecs_lexer_tag_t tag) {
    cortecs_lexer_result_t result = cortecs_lexer_next(in, offset);

    int target_length = strlen(gold);
    cortecs_span_t gold_span = {
        .lines = 0,
        .columns = 0,
    };
    for (uint32_t i = 0;; i++) {
        char c = gold[i];
        if (c == 0) {
            break;
        }

        if (c == '\n') {
            gold_span.columns = 0;
            gold_span.lines++;
        } else {
            gold_span.columns++;
        }
    }

    TEST_ASSERT_EQUAL_INT32(offset + target_length, result.start);
    TEST_ASSERT_EQUAL_INT32(gold_span.lines, result.token.span.lines);
    TEST_ASSERT_EQUAL_INT32(gold_span.columns, result.token.span.columns);
    TEST_ASSERT_TRUE(result.token.tag == tag);
    TEST_ASSERT_TRUE(strncmp(gold, result.token.text, strlen(gold)) == 0);
}

void cortecs_lexer_test_int(void) {
    cortecs_lexer_test("1", 0, "1", CORTECS_LEXER_TAG_INT);
    cortecs_lexer_test("123", 0, "123", CORTECS_LEXER_TAG_INT);
    cortecs_lexer_test("123 asdf", 0, "123", CORTECS_LEXER_TAG_INT);
    cortecs_lexer_test("asdf 123", 5, "123", CORTECS_LEXER_TAG_INT);
    cortecs_lexer_test("asdf 123 qwer", 5, "123", CORTECS_LEXER_TAG_INT);
}

void cortecs_lexer_test_function(void) {
    cortecs_lexer_test("function", 0, "function", CORTECS_LEXER_TAG_FUNCTION);
    cortecs_lexer_test("asdf function", 5, "function", CORTECS_LEXER_TAG_FUNCTION);
    cortecs_lexer_test("function 123", 0, "function", CORTECS_LEXER_TAG_FUNCTION);
    cortecs_lexer_test("asdf function 123", 5, "function", CORTECS_LEXER_TAG_FUNCTION);
}

void cortecs_lexer_test_let(void) {
    cortecs_lexer_test("let", 0, "let", CORTECS_LEXER_TAG_LET);
    cortecs_lexer_test("asdf let", 5, "let", CORTECS_LEXER_TAG_LET);
    cortecs_lexer_test("let 123", 0, "let", CORTECS_LEXER_TAG_LET);
    cortecs_lexer_test("asdf let 123", 5, "let", CORTECS_LEXER_TAG_LET);
}

void cortecs_lexer_test_return(void) {
    cortecs_lexer_test("return", 0, "return", CORTECS_LEXER_TAG_RETURN);
    cortecs_lexer_test("asdf return", 5, "return", CORTECS_LEXER_TAG_RETURN);
    cortecs_lexer_test("return 123", 0, "return", CORTECS_LEXER_TAG_RETURN);
    cortecs_lexer_test("asdf return 123", 5, "return", CORTECS_LEXER_TAG_RETURN);
}

void cortecs_lexer_test_if(void) {
    cortecs_lexer_test("if", 0, "if", CORTECS_LEXER_TAG_IF);
    cortecs_lexer_test("asdf if", 5, "if", CORTECS_LEXER_TAG_IF);
    cortecs_lexer_test("if 123", 0, "if", CORTECS_LEXER_TAG_IF);
    cortecs_lexer_test("asdf if 123", 5, "if", CORTECS_LEXER_TAG_IF);
}

void cortecs_lexer_test_name(void) {
    for (int i = 0; i < 1000; i++) {
        cortecs_lexer_token_t token = cortecs_lexer_fuzz_name();
        cortecs_lexer_test(token.text, 0, token.text, token.tag);
        free(token.text);
    }

    cortecs_lexer_test("a", 0, "a", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("asdf", 0, "asdf", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("a123", 0, "a123", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("a123_b456", 0, "a123_b456", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("123 asdf", 4, "asdf", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("asdf 123", 0, "asdf", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("qwer asdf 123", 5, "asdf", CORTECS_LEXER_TAG_NAME);
}

void cortecs_lexer_test_type(void) {
    cortecs_lexer_test("A", 0, "A", CORTECS_LEXER_TAG_TYPE);
    cortecs_lexer_test("Asdf", 0, "Asdf", CORTECS_LEXER_TAG_TYPE);
    cortecs_lexer_test("A123", 0, "A123", CORTECS_LEXER_TAG_TYPE);
    cortecs_lexer_test("A123_b456", 0, "A123_b456", CORTECS_LEXER_TAG_TYPE);
    cortecs_lexer_test("123 Asdf", 4, "Asdf", CORTECS_LEXER_TAG_TYPE);
    cortecs_lexer_test("Asdf 123", 0, "Asdf", CORTECS_LEXER_TAG_TYPE);
    cortecs_lexer_test("qwer Asdf 123", 5, "Asdf", CORTECS_LEXER_TAG_TYPE);
}

void cortecs_lexer_test_whitespace(void) {
    cortecs_lexer_test(" ", 0, " ", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("\t", 0, "\t", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("\n", 0, "\n", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("  \t\n  ", 0, "  \t\n  ", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("  \t\t  123", 0, "  \t\t  ", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("asdf  \t", 4, "  \t", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("asdf  \n", 4, "  \n", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("asdf  \t123", 4, "  \t", CORTECS_LEXER_TAG_SPACE);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(cortecs_lexer_test_int);
    RUN_TEST(cortecs_lexer_test_function);
    RUN_TEST(cortecs_lexer_test_let);
    RUN_TEST(cortecs_lexer_test_return);
    RUN_TEST(cortecs_lexer_test_if);
    RUN_TEST(cortecs_lexer_test_name);
    RUN_TEST(cortecs_lexer_test_type);
    RUN_TEST(cortecs_lexer_test_whitespace);
    return UNITY_END();
}