#include <common.h>
#include <lexer.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <tokens.h>
#include <unity.h>

#include "test_impls.h"

void setUp() {
    srand(time(NULL));
    // required for unity
}

void tearDown() {
    // required for unity
}

#define CORTECS_LEXER_INT_CHAR_MAX 10

cortecs_lexer_test_result_t lexer_test_int_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    return (cortecs_lexer_test_result_t){
        .next_state = 0,
        .next_char = '0' + entropy % CORTECS_LEXER_INT_CHAR_MAX,
    };
}

uint32_t lexer_test_int_max_entropy(uint32_t state) {
    UNUSED(state);
    return CORTECS_LEXER_INT_CHAR_MAX;
}

void cortecs_lexer_test_int(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_int_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_int_max_entropy,
        .tag = CORTECS_LEXER_TAG_INT,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
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

static bool lexer_text_name_skip(char *token, uint32_t length) {
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

#define CORTECS_LEXER_NAME_FIRST_CHAR_MAX 27
#define CORTECS_LEXER_NAME_VALID_CHAR_MAX 53

cortecs_lexer_test_result_t lexer_test_name_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    cortecs_lexer_test_result_t result = {
        .next_state = 1,
    };

    uint32_t i;
    if (state.state == 0) {
        i = entropy % CORTECS_LEXER_NAME_FIRST_CHAR_MAX;
    } else {
        i = entropy % CORTECS_LEXER_NAME_VALID_CHAR_MAX;
    }

    if (i < 26) {
        result.next_char = 'a' + i;
    } else if (i < CORTECS_LEXER_NAME_FIRST_CHAR_MAX) {
        result.next_char = '_';
    } else {
        result.next_char = 'A' + (i - CORTECS_LEXER_NAME_FIRST_CHAR_MAX);
    }

    return result;
}

uint32_t lexer_test_name_max_entropy(uint32_t state) {
    if (state == 0) {
        return CORTECS_LEXER_NAME_FIRST_CHAR_MAX;
    }

    return CORTECS_LEXER_NAME_VALID_CHAR_MAX;
}

static void lexer_test_name(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_name_next,
        .should_skip_token = &lexer_text_name_skip,
        .state_max_entropy = &lexer_test_name_max_entropy,
        .tag = CORTECS_LEXER_TAG_NAME,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
}

#define CORTECS_LEXER_TYPE_FIRST_CHAR_MAX 26
#define CORTECS_LEXER_TYPE_VALID_CHAR_MAX 53

cortecs_lexer_test_result_t lexer_test_type_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    cortecs_lexer_test_result_t result = {
        .next_state = 1,
    };

    uint32_t i;
    if (state.state == 0) {
        i = entropy % CORTECS_LEXER_TYPE_FIRST_CHAR_MAX;
    } else {
        i = entropy % CORTECS_LEXER_TYPE_VALID_CHAR_MAX;
    }

    if (i < CORTECS_LEXER_TYPE_FIRST_CHAR_MAX) {
        result.next_char = 'A' + i;
    } else if (i < CORTECS_LEXER_TYPE_FIRST_CHAR_MAX + 26) {
        result.next_char = 'a' + (i - CORTECS_LEXER_TYPE_FIRST_CHAR_MAX);
    } else {
        result.next_char = '_';
    }

    return result;
}

uint32_t lexer_test_type_max_entropy(uint32_t state) {
    if (state == 0) {
        return CORTECS_LEXER_TYPE_FIRST_CHAR_MAX;
    }

    return CORTECS_LEXER_TYPE_VALID_CHAR_MAX;
}

static void lexer_test_type(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_type_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_type_max_entropy,
        .tag = CORTECS_LEXER_TAG_TYPE,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
}

#define CORTECS_LEXER_SPACE_CHAR_MAX 5

cortecs_lexer_test_result_t lexer_test_space_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    cortecs_lexer_test_result_t result = {
        .next_state = 0,
    };
    switch (entropy % CORTECS_LEXER_SPACE_CHAR_MAX) {
        case 0:
            result.next_char = ' ';
        case 1:
            result.next_char = '\t';
        case 2:
            result.next_char = '\r';
        case 3:
            result.next_char = '\v';
        default:
            result.next_char = '\f';
    }
    return result;
}

uint32_t lexer_test_space_max_entropy(uint32_t state) {
    UNUSED(state);
    return CORTECS_LEXER_SPACE_CHAR_MAX;
}

static void lexer_test_space(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_space_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_space_max_entropy,
        .tag = CORTECS_LEXER_TAG_SPACE,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
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

    RUN_TEST(lexer_test_space);

    RUN_TEST(cortecs_lexer_test_int);
    RUN_TEST(cortecs_lexer_test_float);
    RUN_TEST(cortecs_lexer_test_function);
    RUN_TEST(cortecs_lexer_test_let);
    RUN_TEST(cortecs_lexer_test_return);
    RUN_TEST(cortecs_lexer_test_if);

    RUN_TEST(lexer_test_type);
    RUN_TEST(lexer_test_name);

    RUN_TEST(cortecs_lexer_test_new_line);

    return UNITY_END();
}