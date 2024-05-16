#include <lexer.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>
#include <unity.h>

#include "exhaustive.h"
#include "fuzz.h"
#include "util.h"

void setUp() {
    // required for unity
}

void tearDown() {
    // required for unity
}

void cortecs_lexer_test_int(void) {
    cortecs_lexer_test("1", 0, "1", CORTECS_LEXER_TAG_INT);
    cortecs_lexer_test("123", 0, "123", CORTECS_LEXER_TAG_INT);
    cortecs_lexer_test("123 asdf", 0, "123", CORTECS_LEXER_TAG_INT);
    cortecs_lexer_test("asdf 123", 5, "123", CORTECS_LEXER_TAG_INT);
    cortecs_lexer_test("asdf 123 qwer", 5, "123", CORTECS_LEXER_TAG_INT);
}

void cortecs_lexer_test_float(void) {
    cortecs_lexer_test("1.", 0, "1.", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test(".1", 0, ".1", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test("123.", 0, "123.", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test(".123", 0, ".123", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test("123. asdf", 0, "123.", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test(".123 asdf", 0, ".123", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test("asdf 123.", 5, "123.", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test("asdf .123", 5, ".123", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test("asdf 123. qwer", 5, "123.", CORTECS_LEXER_TAG_FLOAT);
    cortecs_lexer_test("asdf .123 qwer", 5, ".123", CORTECS_LEXER_TAG_FLOAT);
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

bool lexer_text_name_skip(char *token, uint32_t length) {
    if (length == 2 && strncmp(token, "if", 2) == 0) {
        return true;
    }

    if (length == 3 && strncmp(token, "let", 3) == 0) {
        return true;
    }

    if (length == 6 && strncmp(token, "return", 6) == 0) {
        return true;
    }

    if (length == 8 && strncmp(token, "function", 8) == 0) {
        return true;
    }

    return false;
}

void lexer_test_name_exhaustive(void) {
    // Exhaustive test of short type tokens
    cortecs_lexer_exhaustive_config_t config = {
        .get_first_char = &cortecs_lexer_name_first_char,
        .num_first_char = CORTECS_LEXER_NAME_FIRST_CHAR_MAX,
        .get_other_chars = &cortecs_lexer_name_valid_char,
        .num_other_chars = CORTECS_LEXER_NAME_VALID_CHAR_MAX,
        .get_finalizer_char = &cortecs_lexer_name_type_finalizer_char,
        .num_finalizer_char = CORTECS_LEXER_NAME_TYPE_FINALIZER_CHAR_MAX,
        .should_skip_token = &lexer_text_name_skip,
        .tag = CORTECS_LEXER_TAG_NAME,
    };
    cortecs_lexer_exhaustive_test(config);
}

void cortecs_lexer_test_name_fuzz_single_token(void) {
    // Tests lexing of random strings matching [a-z_][a-zA-Z0-9_]*
    for (int i = 0; i < 1000; i++) {
        cortecs_lexer_token_t token = cortecs_lexer_fuzz_name();
        cortecs_lexer_test(token.text, 0, token.text, token.tag);
        free(token.text);
    }
}

void cortecs_lexer_test_name(void) {
    cortecs_lexer_test("a", 0, "a", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("asdf", 0, "asdf", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("a123", 0, "a123", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("a123_b456", 0, "a123_b456", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("123 asdf", 4, "asdf", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("asdf 123", 0, "asdf", CORTECS_LEXER_TAG_NAME);
    cortecs_lexer_test("qwer asdf 123", 5, "asdf", CORTECS_LEXER_TAG_NAME);
}

void lexer_test_type_exhaustive(void) {
    // Exhaustive test of short type tokens
    cortecs_lexer_exhaustive_config_t config = {
        .get_first_char = &cortecs_lexer_type_first_char,
        .num_first_char = CORTECS_LEXER_TYPE_FIRST_CHAR_MAX,
        .get_other_chars = &cortecs_lexer_type_valid_char,
        .num_other_chars = CORTECS_LEXER_TYPE_VALID_CHAR_MAX,
        .get_finalizer_char = &cortecs_lexer_name_type_finalizer_char,
        .num_finalizer_char = CORTECS_LEXER_NAME_TYPE_FINALIZER_CHAR_MAX,
        .should_skip_token = &cortecs_lexer_exhaustive_never_skip,
        .tag = CORTECS_LEXER_TAG_TYPE,
    };
    cortecs_lexer_exhaustive_test(config);
}

void cortecs_lexer_test_type_fuzz_single_token(void) {
    // Tests lexing of random strings matching [A-Z][a-zA-Z0-9_]*
    for (int i = 0; i < 1000; i++) {
        cortecs_lexer_token_t token = cortecs_lexer_fuzz_type();
        cortecs_lexer_test(token.text, 0, token.text, token.tag);
        free(token.text);
    }
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

void cortecs_lexer_test_space(void) {
    for (int i = 0; i < 1000; i++) {
        cortecs_lexer_token_t token = cortecs_lexer_fuzz_space();
        cortecs_lexer_test(token.text, 0, token.text, token.tag);
        free(token.text);
    }

    cortecs_lexer_test(" ", 0, " ", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("\t", 0, "\t", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("  \t  ", 0, "  \t  ", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("  \t\t  123", 0, "  \t\t  ", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("asdf  \t", 4, "  \t", CORTECS_LEXER_TAG_SPACE);
    cortecs_lexer_test("asdf  \t123", 4, "  \t", CORTECS_LEXER_TAG_SPACE);
}

void cortecs_lexer_test_new_line(void) {
    cortecs_lexer_test("\n", 0, "\n", CORTECS_LEXER_TAG_NEW_LINE);
    cortecs_lexer_test("\n\n", 0, "\n", CORTECS_LEXER_TAG_NEW_LINE);
    cortecs_lexer_test("asdf\n", 4, "\n", CORTECS_LEXER_TAG_NEW_LINE);
    cortecs_lexer_test("asdf\n\n", 4, "\n", CORTECS_LEXER_TAG_NEW_LINE);
    cortecs_lexer_test("\n123", 0, "\n", CORTECS_LEXER_TAG_NEW_LINE);
    cortecs_lexer_test("\n\n123", 0, "\n", CORTECS_LEXER_TAG_NEW_LINE);
    cortecs_lexer_test("asdf\n123", 4, "\n", CORTECS_LEXER_TAG_NEW_LINE);
    cortecs_lexer_test("asdf\n\n123", 4, "\n", CORTECS_LEXER_TAG_NEW_LINE);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(cortecs_lexer_test_int);
    RUN_TEST(cortecs_lexer_test_float);
    RUN_TEST(cortecs_lexer_test_function);
    RUN_TEST(cortecs_lexer_test_let);
    RUN_TEST(cortecs_lexer_test_return);
    RUN_TEST(cortecs_lexer_test_if);

    RUN_TEST(lexer_test_name_exhaustive);
    RUN_TEST(cortecs_lexer_test_name_fuzz_single_token);
    RUN_TEST(cortecs_lexer_test_name);

    RUN_TEST(lexer_test_type_exhaustive);
    RUN_TEST(cortecs_lexer_test_type_fuzz_single_token);
    RUN_TEST(cortecs_lexer_test_type);

    RUN_TEST(cortecs_lexer_test_space);
    RUN_TEST(cortecs_lexer_test_new_line);

    return UNITY_END();
}