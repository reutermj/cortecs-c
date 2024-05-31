#include <assert.h>
#include <ctype.h>
#include <lexer.h>
#include <span.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>

static cortecs_lexer_result_t construct_result(cortecs_lexer_tag_t tag, char *text, uint32_t start, uint32_t end, cortecs_span_t span) {
    char *token = calloc(end - start + 1, sizeof(char));
    memcpy(token, &text[start], end - start);

    return (cortecs_lexer_result_t){
        .start = end,
        .token = {
            .tag = tag,
            .span = span,
            .text = token,
        },
    };
}

static cortecs_lexer_result_t construct_single_char_result(cortecs_lexer_tag_t tag, char *text, uint32_t start) {
    cortecs_span_t span = {
        .lines = 0,
        .columns = 1,
    };
    return construct_result(tag, text, start, start + 1, span);
}

static cortecs_lexer_result_t lex_float_bad(char *text, uint32_t start, uint32_t end) {
    // (\d+\.\d*[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\.\d+[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\d+\.\d*[dD][a-zA-Z0-9_]+) | (\.\d+[dD][a-zA-Z0-9_]+)
    while (true) {
        char current_char = text[end];
        if (isalnum(current_char) || current_char == '_') {
            end++;
            continue;
        }

        break;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_BAD_FLOAT, text, start, end, span);
}

static cortecs_lexer_result_t lex_float(char *text, uint32_t start, uint32_t end) {
    // (\d+\.\d*[dD]?) | (\.\d+[dD]?)
    // lhs case:
    //   * this function is called by lex_int.
    //   * the double suffix can immediately follow the dot.
    //     * lex_int immediately calls this function lexing the dot
    // rhs case:
    //   * this function is called by lex_dot.
    //   * there must be a digit before the double suffix.
    //     * this condition is guaranteed by lex_dot

    while (true) {
        char current_char = text[end];
        if (isdigit(current_char)) {
            end++;
            continue;
        }

        if (current_char == 'd' || current_char == 'D') {
            end++;
            break;
        }

        if (isalpha(current_char) || current_char == '_') {
            return lex_float_bad(text, start, end + 1);
        }

        break;
    }

    char current_char = text[end];
    if (isalnum(current_char) || current_char == '_') {
        return lex_float_bad(text, start, end + 1);
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_FLOAT, text, start, end, span);
}

static cortecs_lexer_result_t lex_dot(char *text, uint32_t start) {
    if (isdigit(text[start + 1])) {
        // the token is a float literal matching \.\d+[dD]?
        return lex_float(text, start, start + 2);
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = 1,
    };

    return construct_result(CORTECS_LEXER_TAG_DOT, text, start, start + 1, span);
}

static cortecs_lexer_result_t lex_int_bad(char *text, uint32_t start, uint32_t end) {
    while (true) {
        char current_char = text[end];
        if (isalnum(current_char) || current_char == '_') {
            end++;
            continue;
        }

        break;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_BAD_INT, text, start, end, span);
}

static cortecs_lexer_result_t lex_int(char *text, uint32_t start) {
    // [0-9]+([uU]?[bBsSlL])?
    uint32_t end = start + 1;
    while (true) {
        char current_char = text[end];
        if (current_char == '.') {
            // the token is a float literal matching \d+\.\d*[dD]?
            return lex_float(text, start, end + 1);
        }

        if (isdigit(current_char)) {
            end++;
            continue;
        }

        if (current_char == 'b' || current_char == 'B' || current_char == 's' || current_char == 'S' || current_char == 'l' || current_char == 'L') {
            end++;
            break;
        }

        if (current_char == 'u' || current_char == 'U') {
            end++;
            current_char = text[end];
            if (current_char == 'b' || current_char == 'B' || current_char == 's' || current_char == 'S' || current_char == 'l' || current_char == 'L') {
                end++;
                break;
            }

            return lex_int_bad(text, start, end);
        }

        break;
    }

    char current_char = text[end];
    if (isalnum(current_char) || current_char == '_') {
        return lex_int_bad(text, start, end + 1);
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_INT, text, start, end, span);
}

#define check_keyword(kw, text, len) (((len) == sizeof(kw) - 1) && strncmp(text, kw, len) == 0)

static cortecs_lexer_result_t lex_name(char *text, uint32_t start) {
    // [a-zA-Z][a-zA-Z0-9_]*
    uint32_t end = start + 1;
    while (true) {
        char current_char = text[end];
        if (isalnum(current_char) || current_char == '_') {
            end++;
            continue;
        }

        break;
    }

    uint32_t len = end - start;
    cortecs_lexer_tag_t tag;

    if (check_keyword("function", &text[start], len)) {
        tag = CORTECS_LEXER_TAG_FUNCTION;
    } else if (check_keyword("let", &text[start], len)) {
        tag = CORTECS_LEXER_TAG_LET;
    } else if (check_keyword("if", &text[start], len)) {
        tag = CORTECS_LEXER_TAG_IF;
    } else if (check_keyword("return", &text[start], len)) {
        tag = CORTECS_LEXER_TAG_RETURN;
    } else if (isupper(text[start])) {
        tag = CORTECS_LEXER_TAG_TYPE;
    } else {
        tag = CORTECS_LEXER_TAG_NAME;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(tag, text, start, end, span);
}

static cortecs_lexer_result_t lex_whitespace(char *text, uint32_t start) {
    // [\ \t\r\f\v]+
    uint32_t end = start + 1;
    while (true) {
        char current_char = text[end];
        if (isspace(current_char) && current_char != '\n') {
            end++;
            continue;
        }

        break;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_SPACE, text, start, end, span);
}

static bool is_operator(char charater) {
    return charater == '!' || charater == '#' || charater == '$' ||
           charater == '%' || charater == '&' || charater == '*' ||
           charater == '+' || charater == '-' || charater == '/' ||
           charater == '<' || charater == '=' || charater == '>' ||
           charater == '?' || charater == '@' || charater == '\\' ||
           charater == '^' || charater == '|' || charater == '~';
}

static cortecs_lexer_result_t lex_operator(char *text, uint32_t start) {
    // [\ \t\r\f\v]+
    uint32_t end = start + 1;
    while (true) {
        char current_char = text[end];
        if (is_operator(current_char)) {
            end++;
            continue;
        }

        break;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_OPERATOR, text, start, end, span);
}

static bool is_invalid(char charater) {
    // ensure that the char is represented as unsigned [0, 255]
    uint8_t as_uint = (uint8_t)charater;

    // first range of control characters
    const uint8_t ctrl0_min = 1;
    const uint8_t ctrl0_max = 8;

    // 9-13 are whitespace

    // second range of control characters
    const uint8_t ctrl1_min = 14;
    const uint8_t ctrl1_max = 31;

    // last control character + extended ascii
    const uint8_t oob_min = 127;
    return (ctrl0_min <= as_uint && as_uint <= ctrl0_max) ||
           (ctrl1_min <= as_uint && as_uint <= ctrl1_max) ||
           as_uint >= oob_min;
}

static cortecs_lexer_result_t lex_invalid(char *text, uint32_t start) {
    uint32_t end = start + 1;
    while (true) {
        char current_char = text[end];
        if (is_invalid(current_char)) {
            end++;
            continue;
        }

        break;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_INVALID, text, start, end, span);
}

cortecs_lexer_result_t cortecs_lexer_next(char *text, uint32_t start) {
    if (text == NULL) {
        cortecs_span_t span = {
            .lines = 0,
            .columns = 0,
        };
        return construct_result(CORTECS_LEXER_TAG_INVALID, "", start, start, span);
    }

    char current_char = text[start];
    switch (current_char) {
        case 0: {
            cortecs_span_t span = {
                .lines = 0,
                .columns = 0,
            };
            return construct_result(CORTECS_LEXER_TAG_INVALID, "", start, start, span);
        }
        case '.': {
            return lex_dot(text, start);
        }
        case '\n': {
            return construct_result(CORTECS_LEXER_TAG_NEW_LINE, text, start, start + 1, (cortecs_span_t){.lines = 1});
        }
        case '(': {
            return construct_single_char_result(CORTECS_LEXER_TAG_OPEN_PAREN, text, start);
        }
        case ')': {
            return construct_single_char_result(CORTECS_LEXER_TAG_CLOSE_PAREN, text, start);
        }
        case '{': {
            return construct_single_char_result(CORTECS_LEXER_TAG_OPEN_CURLY, text, start);
        }
        case '}': {
            return construct_single_char_result(CORTECS_LEXER_TAG_CLOSE_CURLY, text, start);
        }
        case '[': {
            return construct_single_char_result(CORTECS_LEXER_TAG_OPEN_SQUARE, text, start);
        }
        case ']': {
            return construct_single_char_result(CORTECS_LEXER_TAG_CLOSE_SQUARE, text, start);
        }
        case '\'': {
            return construct_single_char_result(CORTECS_LEXER_TAG_SINGLE_QUOTE, text, start);
        }
        case '"': {
            return construct_single_char_result(CORTECS_LEXER_TAG_DOUBLE_QUOTE, text, start);
        }
        case '`': {
            return construct_single_char_result(CORTECS_LEXER_TAG_BACK_QUOTE, text, start);
        }
        case ',': {
            return construct_single_char_result(CORTECS_LEXER_TAG_COMMA, text, start);
        }
        case ':': {
            return construct_single_char_result(CORTECS_LEXER_TAG_COLON, text, start);
        }
        case ';': {
            return construct_single_char_result(CORTECS_LEXER_TAG_SEMICOLON, text, start);
        }
        default: {
            if (isalpha(current_char) || current_char == '_') {
                return lex_name(text, start);
            }

            if (isdigit(current_char)) {
                return lex_int(text, start);
            }

            if (isspace(current_char)) {
                return lex_whitespace(text, start);
            }

            if (is_operator(current_char)) {
                return lex_operator(text, start);
            }

            // This assert is for testing to ensure that all possible characters have been covered
            assert(is_invalid(current_char));

            return lex_invalid(text, start);
        }
    }
}