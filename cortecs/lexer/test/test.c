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
        cortecs_lexer_test_name_config,
        cortecs_lexer_test_type_config,
        cortecs_lexer_test_float_config,
        cortecs_lexer_test_bad_float_config,
        cortecs_lexer_test_int_config,
        cortecs_lexer_test_bad_int_config,
        cortecs_lexer_test_invalid_config,
    };

    uint32_t lengths[8];
    uint32_t **matrix = calloc(8, sizeof(uint32_t *));
    matrix[0] = calloc(7, sizeof(uint32_t));
    matrix[0][0] = 1;
    matrix[0][1] = 2;
    matrix[0][2] = 3;
    matrix[0][3] = 4;
    matrix[0][4] = 5;
    matrix[0][5] = 6;
    matrix[0][6] = 7;
    lengths[0] = 7;

    matrix[1] = calloc(2, sizeof(uint32_t));
    matrix[1][0] = 0;
    matrix[1][1] = 7;
    lengths[1] = 2;

    matrix[2] = calloc(2, sizeof(uint32_t));
    matrix[2][0] = 0;
    matrix[2][1] = 7;
    lengths[2] = 2;

    matrix[3] = calloc(2, sizeof(uint32_t));
    matrix[3][0] = 0;
    matrix[3][1] = 7;
    lengths[3] = 2;

    matrix[4] = calloc(2, sizeof(uint32_t));
    matrix[4][0] = 0;
    matrix[4][1] = 7;
    lengths[4] = 2;

    matrix[5] = calloc(2, sizeof(uint32_t));
    matrix[5][0] = 0;
    matrix[5][1] = 7;
    lengths[5] = 2;

    matrix[6] = calloc(2, sizeof(uint32_t));
    matrix[6][0] = 0;
    matrix[6][1] = 7;
    lengths[6] = 2;

    matrix[7] = calloc(7, sizeof(uint32_t));
    matrix[7][0] = 0;
    matrix[7][0] = 1;
    matrix[7][0] = 2;
    matrix[7][0] = 3;
    matrix[7][0] = 4;
    matrix[7][0] = 5;
    matrix[7][0] = 6;
    lengths[7] = 7;

    cortecs_lexer_test_multi_config_t config = {
        .configs = configs,
        .valid_next_token = matrix,
        .lengths = lengths,
        .num_configs = sizeof(configs) / sizeof(cortecs_lexer_test_config_t),
    };

    cortecs_lexer_test_fuzz_multi(config);
    cortecs_lexer_test_exhaustive_two_token(config);

    for (int i = 0; i < 8; i++) {
        free(matrix[i]);
    }
    free(matrix);
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
    TEST_ASSERT_EQUAL_MEMORY("invalid", cortecs_lexer_tag_to_string(CORTECS_LEXER_TAG_INVALID), 8);
    TEST_ASSERT_EQUAL_MEMORY("unknown", cortecs_lexer_tag_to_string((cortecs_lexer_tag_t)-1), 8);
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(cortecs_lexer_test_multi_token_fuzz);
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

    RUN_TEST(cortecs_lexer_test_function);
    RUN_TEST(cortecs_lexer_test_let);
    RUN_TEST(cortecs_lexer_test_return);
    RUN_TEST(cortecs_lexer_test_if);
    RUN_TEST(cortecs_lexer_test_new_line);

    return UNITY_END();
}

void setUp() {
    srand(time(NULL));
    // required for unity
}

void tearDown() {
    // required for unity
}