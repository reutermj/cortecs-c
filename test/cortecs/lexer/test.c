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
    };

    bool **transition_to = calloc(19, sizeof(bool *));
    transition_to[0] = (bool[]){0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[1] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[2] = (bool[]){1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[3] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[4] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[5] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[6] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[7] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[8] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[9] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[10] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[11] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[12] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[13] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[14] = (bool[]){1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
    transition_to[15] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[16] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[17] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};
    transition_to[18] = (bool[]){1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0};

    cortecs_lexer_test_multi_config_t config = {
        .configs = configs,
        .num_configs = sizeof(configs) / sizeof(cortecs_lexer_test_config_t),
        .transition_to = transition_to,
    };

    cortecs_lexer_test_fuzz_multi(config);
    cortecs_lexer_test_exhaustive_two_token(config);
    free(transition_to);
}

static void lexer_test_tag_string(void) {
    TEST_ASSERT_EQUAL_MEMORY("name", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_NAME), 5);
    TEST_ASSERT_EQUAL_MEMORY("type", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_TYPE), 5);
    TEST_ASSERT_EQUAL_MEMORY("int", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_INT), 4);
    TEST_ASSERT_EQUAL_MEMORY("bad_int", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_BAD_INT), 8);
    TEST_ASSERT_EQUAL_MEMORY("float", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_FLOAT), 6);
    TEST_ASSERT_EQUAL_MEMORY("bad_float", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_BAD_FLOAT), 10);
    TEST_ASSERT_EQUAL_MEMORY("space", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_SPACE), 6);
    TEST_ASSERT_EQUAL_MEMORY("new_line", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_NEW_LINE), 9);
    TEST_ASSERT_EQUAL_MEMORY("function", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_FUNCTION), 9);
    TEST_ASSERT_EQUAL_MEMORY("let", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_LET), 4);
    TEST_ASSERT_EQUAL_MEMORY("if", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_IF), 3);
    TEST_ASSERT_EQUAL_MEMORY("return", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_RETURN), 7);
    TEST_ASSERT_EQUAL_MEMORY("dot", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_DOT), 4);
    TEST_ASSERT_EQUAL_MEMORY("open_paren", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_OPEN_PAREN), 4);
    TEST_ASSERT_EQUAL_MEMORY("close_paren", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_CLOSE_PAREN), 4);
    TEST_ASSERT_EQUAL_MEMORY("open_curly", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_OPEN_CURLY), 4);
    TEST_ASSERT_EQUAL_MEMORY("close_curly", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_CLOSE_CURLY), 4);
    TEST_ASSERT_EQUAL_MEMORY("open_square", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_OPEN_SQUARE), 4);
    TEST_ASSERT_EQUAL_MEMORY("close_square", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_CLOSE_SQUARE), 4);
    TEST_ASSERT_EQUAL_MEMORY("invalid", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_INVALID), 8);
    TEST_ASSERT_EQUAL_MEMORY("unknown", cortecs_lexer_tag_to_string((cortecs_lexer_tag_t)-1), 8);
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