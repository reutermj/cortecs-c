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

cortecs_lexer_test_result_t lexer_test_bad_int_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    switch (state.state) {
        case 0: {
            uint32_t next_state;
            if (state.index == state.length - 2) {
                // if this is the second to last char, transition to a state
                // that can only generate bad suffixes
                next_state = 3;
            } else {
                // otherwise move to a state that can generate anything
                next_state = 1;
            }

            return (cortecs_lexer_test_result_t){
                .next_state = next_state,
                .next_char = '0' + entropy % 10,
            };
        }
        case 1: {
            uint32_t i = entropy % 63;
            char next_char;
            uint32_t next_state;

            if (i < 10) {
                next_char = '0' + i;
                if (state.index == state.length - 2) {
                    next_state = 3;
                } else {
                    next_state = 1;
                }
            } else if (i < 36) {
                next_char = 'a' + (i - 10);
                next_state = 2;
            } else if (i < 62) {
                next_char = 'A' + (i - 36);
                next_state = 2;
            } else {
                next_char = '_';
                next_state = 2;
            }

            return (cortecs_lexer_test_result_t){
                .next_state = next_state,
                .next_char = next_char,
            };
        }
        case 2: {
            uint32_t i = entropy % 63;
            char next_char;

            if (i < 10) {
                next_char = '0' + i;
            } else if (i < 36) {
                next_char = 'a' + (i - 10);
            } else if (i < 62) {
                next_char = 'A' + (i - 36);
            } else {
                next_char = '_';
            }

            return (cortecs_lexer_test_result_t){
                .next_state = 2,
                .next_char = next_char,
            };
        }
        default: {
            uint32_t i = entropy % 47;
            char next_char;

            if (i == 0) {
                next_char = 'a';
            } else if (i < 10) {
                next_char = 'c' + (i - 1);
            } else if (i < 16) {
                next_char = 'm' + (i - 10);
            } else if (i < 23) {
                next_char = 't' + (i - 16);
            } else if (i == 23) {
                next_char = 'A';
            } else if (i < 33) {
                next_char = 'C' + (i - 24);
            } else if (i < 39) {
                next_char = 'M' + (i - 33);
            } else if (i < 46) {
                next_char = 'T' + (i - 39);
            } else {
                next_char = '_';
            }

            return (cortecs_lexer_test_result_t){
                .next_char = next_char,
            };
        }
    }
}

uint32_t lexer_test_bad_int_max_entropy(uint32_t state) {
    switch (state) {
        case 0:
            return 10;
        case 1:
            return 63;
        case 2:
            return 63;
        default:
            return 47;
    }
}

void cortecs_lexer_test_bad_int(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_bad_int_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_bad_int_max_entropy,
        .tag = CORTECS_LEXER_TAG_BAD_INT,
        .min_length = 2,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
}

cortecs_lexer_test_result_t lexer_test_int_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    switch (state.state) {
        case 0: {
            uint32_t next_state;
            if (state.index == state.length - 2) {
                // if this is the second to last char, transition to a state
                // that can only generate digits and signed suffixes
                next_state = 2;
            } else if (state.index == state.length - 3) {
                // if this is the second to last char, transition to a state
                // that can generate digits and unsigned suffixes
                next_state = 1;
            } else {
                // otherwise stay in this state and only generate digits
                next_state = 0;
            }

            return (cortecs_lexer_test_result_t){
                .next_state = next_state,
                .next_char = '0' + entropy % 10,
            };
        }
        case 1: {
            uint32_t i = entropy % 12;
            char next_char;

            switch (i) {
                case 10:
                    next_char = 'u';
                case 11:
                    next_char = 'U';
                default:
                    next_char = '0' + entropy % 10;
            }

            return (cortecs_lexer_test_result_t){
                .next_state = 2,
                .next_char = next_char,
            };
        }
        default: {
            uint32_t i = entropy % 16;
            char next_char;

            switch (i) {
                case 10:
                    next_char = 'b';
                case 11:
                    next_char = 'B';
                case 12:
                    next_char = 's';
                case 13:
                    next_char = 'S';
                case 14:
                    next_char = 'l';
                case 15:
                    next_char = 'L';
                default:
                    next_char = '0' + entropy % 10;
            }

            return (cortecs_lexer_test_result_t){
                .next_char = next_char,
            };
        }
    }
}

uint32_t lexer_test_int_max_entropy(uint32_t state) {
    switch (state) {
        case 0:
            return 10;
        case 1:
            return 12;
        default:
            return 16;
    }
}

void cortecs_lexer_test_int(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_int_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_int_max_entropy,
        .tag = CORTECS_LEXER_TAG_INT,
        .min_length = 1,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
}

cortecs_lexer_test_result_t lexer_test_bad_float_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    switch (state.state) {
        case 0: {
            // generate either a '.' or digit
            uint32_t i = entropy % 11;
            if (i == 10) {
                uint32_t next_state;
                if (state.index == 0) {
                    // the dot is the first character.
                    // transition to a state that only generates a digit
                    // (\.\d+[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\.\d+[dD][a-zA-Z0-9_]+)
                    next_state = 1;
                } else {
                    // otherwise transition to a state that can generate digits, letters, or underscore
                    // (\d+\.\d*[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\d+\.\d*[dD][a-zA-Z0-9_]+)
                    next_state = 2;
                }

                return (cortecs_lexer_test_result_t){
                    .next_char = '.',
                    .next_state = next_state,
                };
            }

            uint32_t next_state;
            if (state.index == state.length - 3) {
                // this digit is the second to last character and no '.' has been generated
                // transition to a state that only generates the '.' then to a state
                // that generates a bad suffix
                // (\d+\.[a-ce-zA-CE-Z_])
                next_state = 4;
            } else {
                // otherwise stay in this state
                // (\d+\.\d*[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\d+\.\d*[dD][a-zA-Z0-9_]+)
                next_state = 0;
            }

            return (cortecs_lexer_test_result_t){
                .next_char = '0' + i,
                .next_state = next_state,
            };
        }
        case 1: {
            // generate a digit. second character of one of the following:
            // (\.\d+[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\.\d+[dD][a-zA-Z0-9_]+)
            uint32_t next_state;
            if (state.index == state.length - 2) {
                // this digit is the second to last character.
                // transition to a state that generates a bad suffix
                // (\.\d[a-ce-zA-CE-Z_])
                next_state = 5;
            } else {
                // otherwise transition to a state that can generate a digit, letter, or underscore
                // (\.\d+[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\.\d+[dD][a-zA-Z0-9_]+)
                next_state = 2;
            }

            return (cortecs_lexer_test_result_t){
                .next_char = '0' + entropy % 10,
                .next_state = next_state,
            };
        }
        case 2: {
            // generate a digit, letter, or underscore
            // this state always comes after the dot has been generated
            uint32_t i = entropy % 63;
            char next_char;
            uint32_t next_state;

            if (i < 10) {
                next_char = '0' + i;
                if (state.index == state.length - 2) {
                    next_state = 5;
                } else {
                    next_state = 2;
                }
            } else if (i < 36) {
                next_char = 'a' + (i - 10);
                next_state = 3;
            } else if (i < 62) {
                next_char = 'A' + (i - 36);
                next_state = 3;
            } else {
                next_char = '_';
                next_state = 3;
            }

            return (cortecs_lexer_test_result_t){
                .next_char = next_char,
                .next_state = next_state,
            };
        }
        case 3: {
            // Already constructed a bad float token.
            // now just need to generate chars until desired length
            // generates [0-9a-zA-Z_]
            uint32_t i = entropy % 63;
            char next_char;

            if (i < 10) {
                next_char = '0' + i;
            } else if (i < 36) {
                next_char = 'a' + (i - 10);
            } else if (i < 62) {
                next_char = 'A' + (i - 36);
            } else {
                next_char = '_';
            }

            return (cortecs_lexer_test_result_t){
                .next_char = next_char,
                .next_state = 3,
            };
        }
        case 4: {
            // generate a dot then transition to a state that generates a bad suffix
            // this is the second last character in the token
            // (\d+\.[a-ce-zA-CE-Z_])
            return (cortecs_lexer_test_result_t){
                .next_char = '.',
                .next_state = 5,
            };
        }
        default: {
            // generates the last character in a token that matches
            // (\d+\.\d*[a-ce-zA-CE-Z_]) | (\.\d+[a-ce-zA-CE-Z_])
            uint32_t i = entropy % 51;
            char next_char;

            if (i < 3) {
                next_char = 'a' + i;
            } else if (i < 25) {
                next_char = 'e' + (i - 3);
            } else if (i < 28) {
                next_char = 'A' + (i - 25);
            } else if (i < 50) {
                next_char = 'E' + (i - 28);
            } else {
                next_char = '_';
            }

            return (cortecs_lexer_test_result_t){
                .next_char = next_char,
            };
        }
    }
}

uint32_t lexer_test_bad_float_max_entropy(uint32_t state) {
    switch (state) {
        case 0:
            return 11;
        case 1:
            return 10;
        case 2:
            return 63;
        case 3:
            return 63;
        case 4:
            return 1;
        default:
            return 51;
    }
}

void cortecs_lexer_test_bad_float(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_bad_float_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_bad_float_max_entropy,
        .tag = CORTECS_LEXER_TAG_BAD_FLOAT,
        .min_length = 3,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
}

cortecs_lexer_test_result_t lexer_test_float_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    switch (state.state) {
        case 0: {
            // generate either a '.' or digit
            uint32_t i = entropy % 11;
            if (i == 10) {
                uint32_t next_state;
                if (state.index == 0) {
                    // the dot is the first character.
                    // transition to a state that only generates a digit
                    next_state = 1;
                } else if (state.index == state.length - 2) {
                    // the dot is the second to last character.
                    // transition to a state that can generate either a digit or double suffix
                    next_state = 2;
                } else {
                    // otherwise transition to a state that only generates a digit
                    next_state = 1;
                }

                return (cortecs_lexer_test_result_t){
                    .next_char = '.',
                    .next_state = next_state,
                };
            }

            uint32_t next_state;
            if (state.index == state.length - 2) {
                // this digit is the second to last character and no '.' has been generated
                // transition to a state that only generates the '.'
                next_state = 3;
            } else {
                // otherwise stay in this state
                next_state = 0;
            }

            return (cortecs_lexer_test_result_t){
                .next_char = '0' + i,
                .next_state = next_state,
            };
        }
        case 1: {
            // generate a digit
            uint32_t next_state;
            if (state.index == state.length - 2) {
                // this digit is the second to last character.
                // transition to a state that can generate either a digit or double suffix
                next_state = 2;
            } else {
                // otherwise transition to a state that only generates a digit
                next_state = 1;
            }

            return (cortecs_lexer_test_result_t){
                .next_char = '0' + entropy % 10,
                .next_state = next_state,
            };
        }
        case 2: {
            // generate either a digit or double suffx
            uint32_t i = entropy % 12;
            switch (i) {
                case 10: {
                    return (cortecs_lexer_test_result_t){
                        .next_char = 'd',
                    };
                }
                case 11: {
                    return (cortecs_lexer_test_result_t){
                        .next_char = 'D',
                    };
                }
                default: {
                    return (cortecs_lexer_test_result_t){
                        .next_char = '0' + i,
                    };
                }
            }
        }
        default: {
            // state 3
            // generate a dot
            // this is the last character in the token
            return (cortecs_lexer_test_result_t){
                .next_char = '.',
            };
        }
    }
}

uint32_t lexer_test_float_max_entropy(uint32_t state) {
    switch (state) {
        case 0:
            return 11;
        case 1:
            return 10;
        case 2:
            return 12;
        default:
            return 1;
    }
}

void cortecs_lexer_test_float(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_float_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_float_max_entropy,
        .tag = CORTECS_LEXER_TAG_FLOAT,
        .min_length = 2,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
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

cortecs_lexer_test_result_t lexer_test_name_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    cortecs_lexer_test_result_t result = {
        .next_state = 1,
    };

    uint32_t i;
    if (state.state == 0) {
        i = entropy % 27;
    } else {
        i = entropy % 63;
    }

    if (i < 26) {
        result.next_char = 'a' + i;
    } else if (i == 26) {
        result.next_char = '_';
    } else if (i < 53) {
        result.next_char = 'A' + (i - 27);
    } else {
        result.next_char = '0' + (i - 53);
    }

    return result;
}

uint32_t lexer_test_name_max_entropy(uint32_t state) {
    if (state == 0) {
        return 27;
    }

    return 63;
}

static void lexer_test_name(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_name_next,
        .should_skip_token = &lexer_text_name_skip,
        .state_max_entropy = &lexer_test_name_max_entropy,
        .tag = CORTECS_LEXER_TAG_NAME,
        .min_length = 1,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
}

cortecs_lexer_test_result_t lexer_test_type_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    cortecs_lexer_test_result_t result = {
        .next_state = 1,
    };

    uint32_t i;
    if (state.state == 0) {
        i = entropy % 26;
    } else {
        i = entropy % 63;
    }

    if (i < 26) {
        result.next_char = 'A' + i;
    } else if (i == 26) {
        result.next_char = '_';
    } else if (i < 53) {
        result.next_char = 'a' + (i - 27);
    } else {
        result.next_char = '0' + (i - 53);
    }

    return result;
}

uint32_t lexer_test_type_max_entropy(uint32_t state) {
    if (state == 0) {
        return 26;
    }

    return 63;
}

static void lexer_test_type(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_type_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_type_max_entropy,
        .tag = CORTECS_LEXER_TAG_TYPE,
        .min_length = 1,
    };
    cortecs_lexer_test_fuzz(stm);
    cortecs_lexer_test_exhaustive(stm);
}

cortecs_lexer_test_result_t lexer_test_space_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    cortecs_lexer_test_result_t result = {
        .next_state = 0,
    };
    switch (entropy % 5) {
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
    return 5;
}

static void lexer_test_space(void) {
    cortecs_lexer_test_config_t stm = {
        .next = &lexer_test_space_next,
        .should_skip_token = &cortecs_lexer_test_never_skip,
        .state_max_entropy = &lexer_test_space_max_entropy,
        .tag = CORTECS_LEXER_TAG_SPACE,
        .min_length = 1,
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
    RUN_TEST(cortecs_lexer_test_bad_int);
    RUN_TEST(cortecs_lexer_test_float);
    RUN_TEST(cortecs_lexer_test_bad_float);

    RUN_TEST(cortecs_lexer_test_function);
    RUN_TEST(cortecs_lexer_test_let);
    RUN_TEST(cortecs_lexer_test_return);
    RUN_TEST(cortecs_lexer_test_if);

    RUN_TEST(lexer_test_type);
    RUN_TEST(lexer_test_name);

    RUN_TEST(cortecs_lexer_test_new_line);

    return UNITY_END();
}