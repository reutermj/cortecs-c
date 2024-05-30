#include <common.h>
#include <ctype.h>
#include <lexer.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <tokens.h>
#include <unity.h>

#include "test_configs.h"
#include "test_impls.h"

static void lexer_test_empty_input(void) {
    cortecs_lexer_test("", 0, "", CORTECS_LEXER_TAG_INVALID);
    cortecs_lexer_test("asdf", 4, "", CORTECS_LEXER_TAG_INVALID);
    cortecs_lexer_test("asdf123", 7, "", CORTECS_LEXER_TAG_INVALID);
}

static void lexer_test_dot(void) {
    cortecs_lexer_test(".", 0, ".", CORTECS_LEXER_TAG_DOT);
    cortecs_lexer_test("asdf .", 5, ".", CORTECS_LEXER_TAG_DOT);
    cortecs_lexer_test(".asdf", 0, ".", CORTECS_LEXER_TAG_DOT);
    cortecs_lexer_test("asdf .asdf", 5, ".", CORTECS_LEXER_TAG_DOT);
}

static void lexer_test_float(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_float_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_float_config);
}

static void lexer_test_bad_float(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_bad_float_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_bad_float_config);
}

static void lexer_test_int(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_int_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_int_config);
}

static void lexer_test_bad_int(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_bad_int_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_bad_int_config);
}

void lexer_test_function(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_function_config);
}

void lexer_test_let(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_let_config);
}

void lexer_test_return(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_return_config);
}

void lexer_test_if(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_if_config);
}

static void lexer_test_name(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_name_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_name_config);
}

static void lexer_test_type(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_type_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_type_config);
}

static void lexer_test_space(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_space_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_space_config);
}

static void lexer_test_operator(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_operator_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_operator_config);
}

void lexer_test_new_line(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_new_line_config);
}

static void lexer_test_open_paren(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_open_paren_config);
}

static void lexer_test_close_paren(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_close_paren_config);
}

static void lexer_test_open_curly(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_open_curly_config);
}

static void lexer_test_close_curly(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_close_curly_config);
}

static void lexer_test_open_square(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_open_square_config);
}

static void lexer_test_close_square(void) {
    cortecs_lexer_test_exhaustive(cortecs_lexer_test_close_square_config);
}

static void lexer_test_invalid(void) {
    // currently not running exhaustiveness testing because it takes way too long
    // maybe reenable them if I ever decide it's worth the effort to change test
    // implementations for this case
    // cortecs_lexer_test_exhaustive(cortecs_lexer_test_invalid_config);
    cortecs_lexer_test_fuzz(cortecs_lexer_test_invalid_config);
}

void cortecs_lexer_test_multi_token_fuzz(void) {
    cortecs_lexer_test_config_t configs[] = {
        cortecs_lexer_test_space_config,
        cortecs_lexer_test_new_line_config,
        cortecs_lexer_test_invalid_config,
        cortecs_lexer_test_name_config,
        cortecs_lexer_test_type_config,
        cortecs_lexer_test_float_config,
        cortecs_lexer_test_bad_float_config,
        cortecs_lexer_test_int_config,
        cortecs_lexer_test_bad_int_config,
        cortecs_lexer_test_open_paren_config,
        cortecs_lexer_test_close_paren_config,
        cortecs_lexer_test_open_curly_config,
        cortecs_lexer_test_close_curly_config,
        cortecs_lexer_test_open_square_config,
        cortecs_lexer_test_close_square_config,
        cortecs_lexer_test_function_config,
        cortecs_lexer_test_let_config,
        cortecs_lexer_test_return_config,
        cortecs_lexer_test_if_config,
        cortecs_lexer_test_operator_config,
    };

    bool **transition_to = calloc(20, sizeof(bool *));
    transition_to[0] = (bool[]){0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[1] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[2] = (bool[]){1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[3] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[4] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[5] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[6] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[7] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[8] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[9] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[10] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[11] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[12] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[13] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[14] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[15] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[16] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[17] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[18] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 1};
    transition_to[19] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0};

    cortecs_lexer_test_multi_config_t config = {
        .configs = configs,
        .num_configs = sizeof(configs) / sizeof(cortecs_lexer_test_config_t),
        .transition_to = transition_to,
    };

    cortecs_lexer_test_fuzz_multi(config);
    cortecs_lexer_test_exhaustive_two_token(config);
    free(transition_to);
}

void assert_tag_equals(const char *gold, cortecs_lexer_tag_t tag) {
    uint32_t length = strnlen(gold, 32) + 1;
    const char *out = cortecs_lexer_tag_to_string(tag);
    TEST_ASSERT_EQUAL_MEMORY(gold, out, length);
}

static void lexer_test_tag_string(void) {
    assert_tag_equals("name", CORTECS_LEXER_TAG_NAME);
    assert_tag_equals("type", CORTECS_LEXER_TAG_TYPE);
    assert_tag_equals("int", CORTECS_LEXER_TAG_INT);
    assert_tag_equals("bad_int", CORTECS_LEXER_TAG_BAD_INT);
    assert_tag_equals("float", CORTECS_LEXER_TAG_FLOAT);
    assert_tag_equals("name", CORTECS_LEXER_TAG_NAME);
    assert_tag_equals("bad_float", CORTECS_LEXER_TAG_BAD_FLOAT);
    assert_tag_equals("operator", CORTECS_LEXER_TAG_OPERATOR);
    assert_tag_equals("space", CORTECS_LEXER_TAG_SPACE);
    assert_tag_equals("new_line", CORTECS_LEXER_TAG_NEW_LINE);
    assert_tag_equals("function", CORTECS_LEXER_TAG_FUNCTION);
    assert_tag_equals("let", CORTECS_LEXER_TAG_LET);
    assert_tag_equals("if", CORTECS_LEXER_TAG_IF);
    assert_tag_equals("return", CORTECS_LEXER_TAG_RETURN);
    assert_tag_equals("dot", CORTECS_LEXER_TAG_DOT);
    assert_tag_equals("open_paren", CORTECS_LEXER_TAG_OPEN_PAREN);
    assert_tag_equals("close_paren", CORTECS_LEXER_TAG_CLOSE_PAREN);
    assert_tag_equals("open_curly", CORTECS_LEXER_TAG_OPEN_CURLY);
    assert_tag_equals("close_curly", CORTECS_LEXER_TAG_CLOSE_CURLY);
    assert_tag_equals("open_square", CORTECS_LEXER_TAG_OPEN_SQUARE);
    assert_tag_equals("close_square", CORTECS_LEXER_TAG_CLOSE_SQUARE);
    assert_tag_equals("invalid", CORTECS_LEXER_TAG_INVALID);
    assert_tag_equals("unknown", (cortecs_lexer_tag_t)-1);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(cortecs_lexer_test_multi_token_fuzz);

    RUN_TEST(lexer_test_open_paren);
    RUN_TEST(lexer_test_close_paren);
    RUN_TEST(lexer_test_open_curly);
    RUN_TEST(lexer_test_close_curly);
    RUN_TEST(lexer_test_open_square);
    RUN_TEST(lexer_test_close_square);

    RUN_TEST(lexer_test_tag_string);
    RUN_TEST(lexer_test_empty_input);
    RUN_TEST(lexer_test_dot);

    RUN_TEST(lexer_test_space);
    RUN_TEST(lexer_test_operator);

    RUN_TEST(lexer_test_name);
    RUN_TEST(lexer_test_type);

    RUN_TEST(lexer_test_int);
    RUN_TEST(lexer_test_bad_int);
    RUN_TEST(lexer_test_float);
    RUN_TEST(lexer_test_bad_float);

    RUN_TEST(lexer_test_invalid);

    RUN_TEST(lexer_test_function);
    RUN_TEST(lexer_test_let);
    RUN_TEST(lexer_test_return);
    RUN_TEST(lexer_test_if);
    RUN_TEST(lexer_test_new_line);

    return UNITY_END();
}

void setUp() {
    srand(time(NULL));
    // required for unity
}

void tearDown() {
    // required for unity
}