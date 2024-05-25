#include <common.h>
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

void cortecs_lexer_test_multi_token_fuzz(void) {
    cortecs_lexer_test_config_t configs[] = {
        cortecs_lexer_test_space_config,
        cortecs_lexer_test_name_config,
        cortecs_lexer_test_type_config,
        cortecs_lexer_test_float_config,
        cortecs_lexer_test_bad_float_config,
        cortecs_lexer_test_int_config,
        cortecs_lexer_test_bad_int_config,
    };

    uint32_t lengths[7];
    uint32_t **matrix = calloc(7, sizeof(uint32_t *));
    matrix[0] = calloc(6, sizeof(uint32_t));
    matrix[0][0] = 1;
    matrix[0][1] = 2;
    matrix[0][2] = 3;
    matrix[0][3] = 4;
    matrix[0][4] = 5;
    matrix[0][5] = 6;
    lengths[0] = 6;

    matrix[1] = calloc(1, sizeof(uint32_t));
    matrix[1][0] = 0;
    lengths[1] = 1;

    matrix[2] = calloc(1, sizeof(uint32_t));
    matrix[2][0] = 0;
    lengths[2] = 1;

    matrix[3] = calloc(1, sizeof(uint32_t));
    matrix[3][0] = 0;
    lengths[3] = 1;

    matrix[4] = calloc(1, sizeof(uint32_t));
    matrix[4][0] = 0;
    lengths[4] = 1;

    matrix[5] = calloc(1, sizeof(uint32_t));
    matrix[5][0] = 0;
    lengths[5] = 1;

    matrix[6] = calloc(1, sizeof(uint32_t));
    matrix[6][0] = 0;
    lengths[6] = 1;

    cortecs_lexer_test_fuzz_config_t config = {
        .configs = configs,
        .valid_next_token = matrix,
        .lengths = lengths,
    };

    cortecs_lexer_test_fuzz_multi(config);

    for (int i = 0; i < 7; i++) {
        free(matrix[i]);
    }
    free(matrix);
}

int main() {
    UNITY_BEGIN();
    RUN_TEST(lexer_test_empty_input);
    RUN_TEST(lexer_test_dot);
    RUN_TEST(cortecs_lexer_test_multi_token_fuzz);

    RUN_TEST(lexer_test_space);
    RUN_TEST(lexer_test_name);
    RUN_TEST(lexer_test_type);
    RUN_TEST(lexer_test_int);
    RUN_TEST(lexer_test_bad_int);
    RUN_TEST(lexer_test_float);
    RUN_TEST(lexer_test_bad_float);

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