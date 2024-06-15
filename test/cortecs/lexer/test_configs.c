#include "test_configs.h"

#include <common.h>
#include <ctype.h>
#include <tokens.h>

static const char space_lookup[] = {' ', '\t', '\r', '\v', '\f'};
static uint32_t lexer_test_space_max_entropy(uint32_t state) {
    UNUSED(state);
    return sizeof(space_lookup) / sizeof(char);
}

static cortecs_lexer_test_result_t lexer_test_space_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    entropy = entropy % lexer_test_space_max_entropy(state.state);
    return (cortecs_lexer_test_result_t){
        .next_state = 0,
        .next_char = space_lookup[entropy],
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_space_config = {
    .next = &lexer_test_space_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_space_max_entropy,
    .tag = CORTECS_LEXER_TAG_SPACE,
    .min_length = 1,
    .max_length = 0xFFFFFFFF,
};

static bool lexer_text_name_skip(const char *token, uint32_t length) {
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

static const char lexer_test_name_lookup[] = {
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '_',
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

static uint32_t lexer_test_name_max_entropy(uint32_t state) {
    if (state == 0) {
        return 27;
    }

    return sizeof(lexer_test_name_lookup) / sizeof(char);
}

static cortecs_lexer_test_result_t lexer_test_name_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    entropy = entropy % lexer_test_name_max_entropy(state.state);
    return (cortecs_lexer_test_result_t){
        .next_char = lexer_test_name_lookup[entropy],
        .next_state = 1,
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_name_config = {
    .next = &lexer_test_name_next,
    .should_skip_token = &lexer_text_name_skip,
    .state_max_entropy = &lexer_test_name_max_entropy,
    .tag = CORTECS_LEXER_TAG_NAME,
    .min_length = 1,
    .max_length = 0xFFFFFFFF,
};

static const char lexer_test_type_lookup[] = {
    'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    '_',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9'};

static uint32_t lexer_test_type_max_entropy(uint32_t state) {
    if (state == 0) {
        return 26;
    }

    return sizeof(lexer_test_type_lookup) / sizeof(char);
}

static cortecs_lexer_test_result_t lexer_test_type_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    entropy = entropy % lexer_test_type_max_entropy(state.state);
    return (cortecs_lexer_test_result_t){
        .next_char = lexer_test_type_lookup[entropy],
        .next_state = 1,
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_type_config = {
    .next = &lexer_test_type_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_type_max_entropy,
    .tag = CORTECS_LEXER_TAG_TYPE,
    .min_length = 1,
    .max_length = 0xFFFFFFFF,
};

static const char lexer_test_float_lookup[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'd', 'D'};

static uint32_t lexer_test_float_max_entropy(uint32_t state) {
    switch (state) {
        case 0: {
            const uint32_t num_digits_or_dot = 11;
            return num_digits_or_dot;
        }
        case 1: {
            const uint32_t num_digits = 10;
            return num_digits;
        }
        case 2: {
            const uint32_t num_digits_or_suffix = 12;
            return num_digits_or_suffix;
        }
        default: {
            const uint32_t only_dot = 12;
            return only_dot;
        }
    }
}

static cortecs_lexer_test_result_t lexer_test_float_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    entropy = entropy % lexer_test_float_max_entropy(state.state);
    switch (state.state) {
        case 0: {
            // generate either a '.' or digit
            if (entropy == 10) {
                uint32_t next_state;
                if (state.index != 0 && state.index == state.length - 2) {
                    // the dot is the second to last character but not the first character
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
                .next_char = lexer_test_float_lookup[entropy],
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
                .next_char = lexer_test_float_lookup[entropy],
                .next_state = next_state,
            };
        }
        case 2: {
            // generate either a digit or double suffx
            return (cortecs_lexer_test_result_t){
                .next_char = lexer_test_float_lookup[entropy],
            };
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

cortecs_lexer_test_config_t cortecs_lexer_test_float_config = {
    .next = &lexer_test_float_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_float_max_entropy,
    .tag = CORTECS_LEXER_TAG_FLOAT,
    .min_length = 2,
    .max_length = 0xFFFFFFFF,
};

static uint32_t lexer_test_bad_float_max_entropy(uint32_t state) {
    switch (state) {
        case 0:
            return 11;
        case 1:
            return 10;
        case 2:
        case 3:
            return 63;
        case 4:
            return 1;
        default:
            return 51;
    }
}

static cortecs_lexer_test_result_t lexer_test_bad_float_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    static const char any_char[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        '_'};
    entropy = entropy % lexer_test_bad_float_max_entropy(state.state);
    switch (state.state) {
        case 0: {
            // generate either a '.' or digit
            if (entropy == 10) {
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
                .next_char = any_char[entropy],
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
                .next_char = any_char[entropy],
                .next_state = next_state,
            };
        }
        case 2: {
            // generate a digit, letter, or underscore
            // this state always comes after the dot has been generated
            uint32_t next_state;
            if (entropy < 10) {
                if (state.index == state.length - 2) {
                    next_state = 5;
                } else {
                    next_state = 2;
                }
            } else {
                next_state = 3;
            }

            return (cortecs_lexer_test_result_t){
                .next_char = any_char[entropy],
                .next_state = next_state,
            };
        }
        case 3: {
            // Already constructed a bad float token.
            // now just need to generate chars until desired length
            // generates [0-9a-zA-Z_]

            return (cortecs_lexer_test_result_t){
                .next_char = any_char[entropy],
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
            static const char bad_suffix[] = {
                'a', 'b', 'c', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                'A', 'B', 'C', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                '_'};
            return (cortecs_lexer_test_result_t){
                .next_char = bad_suffix[entropy],
            };
        }
    }
}

cortecs_lexer_test_config_t cortecs_lexer_test_bad_float_config = {
    .next = &lexer_test_bad_float_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_bad_float_max_entropy,
    .tag = CORTECS_LEXER_TAG_BAD_FLOAT,
    .min_length = 3,
    .max_length = 0xFFFFFFFF,
};

static uint32_t lexer_test_int_max_entropy(uint32_t state) {
    switch (state) {
        case 0:
            return 10;
        case 1:
            return 12;
        case 2:
            return 16;
        default:
            return 6;
    }
}

static cortecs_lexer_test_result_t lexer_test_int_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    static const char digits_and_signed_suffixes[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'b', 'B', 's', 'S', 'l', 'L'};
    entropy = entropy % lexer_test_int_max_entropy(state.state);
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
                .next_char = digits_and_signed_suffixes[entropy],
            };
        }
        case 1: {
            static const char digits_and_unsigned_suffixes[] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'u', 'U'};

            uint32_t next_state;
            if (entropy < 10) {
                next_state = 2;
            } else {
                next_state = 3;
            }

            return (cortecs_lexer_test_result_t){
                .next_state = next_state,
                .next_char = digits_and_unsigned_suffixes[entropy],
            };
        }
        case 2: {
            return (cortecs_lexer_test_result_t){
                .next_char = digits_and_signed_suffixes[entropy],
            };
        }
        default: {
            return (cortecs_lexer_test_result_t){
                .next_char = digits_and_signed_suffixes[entropy + 10],
            };
        }
    }
}

cortecs_lexer_test_config_t cortecs_lexer_test_int_config = {
    .next = &lexer_test_int_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_int_max_entropy,
    .tag = CORTECS_LEXER_TAG_INT,
    .min_length = 1,
    .max_length = 0xFFFFFFFF,
};

static uint32_t lexer_test_bad_int_max_entropy(uint32_t state) {
    switch (state) {
        case 0:
            return 10;
        case 1:
        case 2:
            return 63;
        default:
            return 47;
    }
}

static cortecs_lexer_test_result_t lexer_test_bad_int_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    entropy = entropy % lexer_test_bad_int_max_entropy(state.state);
    static const char any_char[] = {
        '0', '1', '2', '3', '4', '5', '6', '7', '8', '9',
        'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm', 'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
        'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
        '_'};
    switch (state.state) {
        case 0: {
            uint32_t next_state;
            if (state.index == state.length - 2) {
                // if this is the second to last char, transition to a state
                // that can only generate bad suffixes
                // matches [0-9][ac-km-rt-zAC-KM-RT-Z_]
                next_state = 3;
            } else {
                // otherwise move to a state that can generate anything
                next_state = 1;
            }

            return (cortecs_lexer_test_result_t){
                .next_state = next_state,
                .next_char = (char)('0' + entropy % 10),
            };
        }
        case 1: {
            char next_char = any_char[entropy];

            uint32_t next_state;
            if (isdigit(next_char)) {
                if (state.index == state.length - 2) {
                    // this digit is the second to last charater.
                    // transition to a state that produces invalid unisigned suffixes
                    // matches: [0-9]+[ac-km-rt-zAC-KM-RT-Z_]
                    next_state = 3;
                } else {
                    // otherwise stay in this state
                    next_state = 1;
                }
            } else if (next_char == 'u' || next_char == 'U') {
                if (state.index == state.length - 2) {
                    // 'u' is the second to last charater.
                    // transition to a state that produces invalid unisigned suffixes
                    // matches: [0-9]+u[ac-km-rt-zAC-KM-RT-Z_]
                    next_state = 3;
                } else {
                    // otherwise the token is already bad, so transition to the
                    // state that produces anything
                    next_state = 2;
                }
            } else {
                next_state = 2;
            }

            return (cortecs_lexer_test_result_t){
                .next_state = next_state,
                .next_char = next_char,
            };
        }
        case 2: {
            // A bad int has already been generated. Generate anything
            // matches: [0-9]+(([uU][a-zA-Z_])|([ac-km-rt-zAC-KM-RT-Z_]))[a-zA-Z0-9_]*
            return (cortecs_lexer_test_result_t){
                .next_state = 2,
                .next_char = any_char[entropy],
            };
        }
        default: {
            // only generates a bad suffix for the last char
            // matches: [0-9]+[uU]?[ac-km-rt-zAC-KM-RT-Z_]
            static const char bad_suffix[] = {
                'a', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'm', 'n', 'o', 'p', 'q', 'r', 't', 'u', 'v', 'w', 'x', 'y', 'z',
                'A', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'M', 'N', 'O', 'P', 'Q', 'R', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
                '_'};

            return (cortecs_lexer_test_result_t){
                .next_char = bad_suffix[entropy],
            };
        }
    }
}

cortecs_lexer_test_config_t cortecs_lexer_test_bad_int_config = {
    .next = &lexer_test_bad_int_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_bad_int_max_entropy,
    .tag = CORTECS_LEXER_TAG_BAD_INT,
    .min_length = 2,
    .max_length = 0xFFFFFFFF,
};

static const char invalid_chars[] = {
    (char)1, (char)2, (char)3, (char)4, (char)5, (char)6, (char)7, (char)8,
    (char)14, (char)15, (char)16, (char)17, (char)18, (char)19, (char)20, (char)21, (char)22,
    (char)23, (char)24, (char)25, (char)26, (char)27, (char)28, (char)29, (char)30, (char)31};
#define NUM_INVALID_CHARS (sizeof(invalid_chars) / sizeof(char))

static uint32_t lexer_test_invalid_max_entropy(uint32_t state) {
    UNUSED(state);
    return NUM_INVALID_CHARS;
}

static cortecs_lexer_test_result_t lexer_test_invalid_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    entropy = entropy % lexer_test_invalid_max_entropy(state.state);
    cortecs_lexer_test_result_t result = {
        .next_char = invalid_chars[entropy],
        .next_state = 0,
    };

    return result;
}

cortecs_lexer_test_config_t cortecs_lexer_test_invalid_config = {
    .next = &lexer_test_invalid_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_invalid_max_entropy,
    .tag = CORTECS_LEXER_TAG_INVALID,
    .min_length = 1,
    .max_length = 0xFFFFFFFF,
};

static uint32_t lexer_test_new_line_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_new_line_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '\n',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_new_line_config = {
    .next = &lexer_test_new_line_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_new_line_max_entropy,
    .tag = CORTECS_LEXER_TAG_NEW_LINE,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_open_paren_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_open_paren_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '(',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_open_paren_config = {
    .next = &lexer_test_open_paren_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_open_paren_max_entropy,
    .tag = CORTECS_LEXER_TAG_OPEN_PAREN,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_close_paren_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_close_paren_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = ')',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_close_paren_config = {
    .next = &lexer_test_close_paren_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_close_paren_max_entropy,
    .tag = CORTECS_LEXER_TAG_CLOSE_PAREN,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_open_curly_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_open_curly_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '{',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_open_curly_config = {
    .next = &lexer_test_open_curly_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_open_curly_max_entropy,
    .tag = CORTECS_LEXER_TAG_OPEN_CURLY,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_close_curly_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_close_curly_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '}',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_close_curly_config = {
    .next = &lexer_test_close_curly_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_close_curly_max_entropy,
    .tag = CORTECS_LEXER_TAG_CLOSE_CURLY,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_open_square_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_open_square_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '[',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_open_square_config = {
    .next = &lexer_test_open_square_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_open_square_max_entropy,
    .tag = CORTECS_LEXER_TAG_OPEN_SQUARE,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_close_square_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_close_square_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = ']',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_close_square_config = {
    .next = &lexer_test_close_square_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_close_square_max_entropy,
    .tag = CORTECS_LEXER_TAG_CLOSE_SQUARE,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_single_quote_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_single_quote_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '\'',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_single_quote_config = {
    .next = &lexer_test_single_quote_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_single_quote_max_entropy,
    .tag = CORTECS_LEXER_TAG_SINGLE_QUOTE,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_double_quote_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_double_quote_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '"',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_double_quote_config = {
    .next = &lexer_test_double_quote_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_double_quote_max_entropy,
    .tag = CORTECS_LEXER_TAG_DOUBLE_QUOTE,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_back_quote_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_back_quote_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '`',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_back_quote_config = {
    .next = &lexer_test_back_quote_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_back_quote_max_entropy,
    .tag = CORTECS_LEXER_TAG_BACK_QUOTE,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_function_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_function_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(entropy);
    static const char *function_keyword = "function";
    return (cortecs_lexer_test_result_t){
        .next_char = function_keyword[state.state],
        .next_state = state.state + 1,
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_function_config = {
    .next = &lexer_test_function_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_function_max_entropy,
    .tag = CORTECS_LEXER_TAG_FUNCTION,
    .min_length = 8,
    .max_length = 8,
};

static uint32_t lexer_test_let_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_let_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(entropy);
    static const char *let_keyword = "let";
    return (cortecs_lexer_test_result_t){
        .next_char = let_keyword[state.state],
        .next_state = state.state + 1,
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_let_config = {
    .next = &lexer_test_let_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_let_max_entropy,
    .tag = CORTECS_LEXER_TAG_LET,
    .min_length = 3,
    .max_length = 3,
};

static uint32_t lexer_test_return_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_return_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(entropy);
    static const char *return_keyword = "return";
    return (cortecs_lexer_test_result_t){
        .next_char = return_keyword[state.state],
        .next_state = state.state + 1,
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_return_config = {
    .next = &lexer_test_return_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_return_max_entropy,
    .tag = CORTECS_LEXER_TAG_RETURN,
    .min_length = 6,
    .max_length = 6,
};

static uint32_t lexer_test_if_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_if_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(entropy);
    static const char *if_keyword = "if";
    return (cortecs_lexer_test_result_t){
        .next_char = if_keyword[state.state],
        .next_state = state.state + 1,
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_if_config = {
    .next = &lexer_test_if_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_if_max_entropy,
    .tag = CORTECS_LEXER_TAG_IF,
    .min_length = 2,
    .max_length = 2,
};

static const char operator_lookup[] = {'!', '#', '$', '%', '&', '*', '+', '-', '/', '<', '=', '>', '?', '@', '\\', '^', '|', '~'};
static uint32_t lexer_test_operator_max_entropy(uint32_t state) {
    UNUSED(state);
    return sizeof(operator_lookup) / sizeof(char);
}

static cortecs_lexer_test_result_t lexer_test_operator_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    entropy = entropy % lexer_test_operator_max_entropy(state.state);
    return (cortecs_lexer_test_result_t){
        .next_state = 0,
        .next_char = operator_lookup[entropy],
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_operator_config = {
    .next = &lexer_test_operator_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_operator_max_entropy,
    .tag = CORTECS_LEXER_TAG_OPERATOR,
    .min_length = 1,
    .max_length = 0xFFFFFFFF,
};

static uint32_t lexer_test_comma_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_comma_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = ',',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_comma_config = {
    .next = &lexer_test_comma_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_comma_max_entropy,
    .tag = CORTECS_LEXER_TAG_COMMA,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_colon_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_colon_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = ':',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_colon_config = {
    .next = &lexer_test_colon_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_colon_max_entropy,
    .tag = CORTECS_LEXER_TAG_COLON,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_semicolon_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_semicolon_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = ';',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_semicolon_config = {
    .next = &lexer_test_semicolon_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_semicolon_max_entropy,
    .tag = CORTECS_LEXER_TAG_SEMICOLON,
    .min_length = 1,
    .max_length = 1,
};

static uint32_t lexer_test_dot_max_entropy(uint32_t state) {
    UNUSED(state);
    return 1;
}

static cortecs_lexer_test_result_t lexer_test_dot_next(cortecs_lexer_test_state_t state, uint32_t entropy) {
    UNUSED(state);
    UNUSED(entropy);
    return (cortecs_lexer_test_result_t){
        .next_char = '.',
    };
}

cortecs_lexer_test_config_t cortecs_lexer_test_dot_config = {
    .next = &lexer_test_dot_next,
    .should_skip_token = &cortecs_lexer_test_never_skip,
    .state_max_entropy = &lexer_test_dot_max_entropy,
    .tag = CORTECS_LEXER_TAG_DOT,
    .min_length = 1,
    .max_length = 1,
};