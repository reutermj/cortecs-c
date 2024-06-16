#include <assert.h>
#include <ctype.h>
#include <lexer.h>
#include <span.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <tokens.h>
#include <unicode/uchar.h>
#include <unicode/umachine.h>
#include <unicode/urename.h>
#include <unicode/utext.h>
#include <unicode/utf16.h>
#include <unicode/utypes.h>

typedef struct {
    UText *text;
    int64_t start;
    int32_t u8_length;
    int32_t num_codepoints;
    cortecs_span_t span;
} lexer_state_t;

static UChar32 current_codepoint(lexer_state_t *state) {
    return utext_current32(state->text);
}

static void next_codepoint(lexer_state_t *state) {
    utext_next32(state->text);
}

static void accumulate_codepoint(lexer_state_t *state, UChar32 codepoint) {
    next_codepoint(state);
    state->u8_length += U8_LENGTH(codepoint);
    state->num_codepoints++;

    // characters that take multiple code points seem to have a span that
    // based on number of codepointers and not a single character
    // example: before and after ไ้ is an increase of 2 in the column
    state->span.columns++;
}

static cortecs_lexer_token_t construct_result(cortecs_lexer_tag_t tag, lexer_state_t *state) {
    // allocate a buffer for the utf-8 encoding of the token + null terminator
    uint8_t *text = calloc(state->u8_length + 1, sizeof(uint8_t));

    // reset the UText to the start of this token and copy it into the buffer
    utext_setNativeIndex(state->text, state->start);
    int32_t next_offset = 0;
    for (int i = 0; i < state->num_codepoints; i++) {
        UChar32 codepoint = current_codepoint(state);
        U8_APPEND_UNSAFE(text, next_offset, codepoint);
        next_codepoint(state);
    }

    return (cortecs_lexer_token_t){
        .tag = tag,
        .span = state->span,
        .text = text,
    };
}

static cortecs_lexer_token_t lex_float_bad(lexer_state_t *state) {
    // (\d+\.\d*[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\.\d+[a-ce-zA-CE-Z_][a-zA-Z0-9_]*) | (\d+\.\d*[dD][a-zA-Z0-9_]+) | (\.\d+[dD][a-zA-Z0-9_]+)
    while (true) {
        UChar32 codepoint = current_codepoint(state);
        if (u_isalnum(codepoint) || codepoint == '_') {
            accumulate_codepoint(state, codepoint);
            continue;
        }

        break;
    }

    return construct_result(CORTECS_LEXER_TAG_BAD_FLOAT, state);
}

static cortecs_lexer_token_t lex_float(lexer_state_t *state) {
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
        UChar32 codepoint = current_codepoint(state);
        if (u_isdigit(codepoint)) {
            accumulate_codepoint(state, codepoint);
            continue;
        }

        if (codepoint == 'd' || codepoint == 'D') {
            accumulate_codepoint(state, codepoint);
            break;
        }

        if (u_isalpha(codepoint) || codepoint == '_') {
            accumulate_codepoint(state, codepoint);
            return lex_float_bad(state);
        }

        break;
    }

    UChar32 codepoint = current_codepoint(state);
    if (u_isalnum(codepoint) || codepoint == '_') {
        accumulate_codepoint(state, codepoint);
        return lex_float_bad(state);
    }

    return construct_result(CORTECS_LEXER_TAG_FLOAT, state);
}

static cortecs_lexer_token_t lex_dot(lexer_state_t *state) {
    UChar32 codepoint = current_codepoint(state);
    if (u_isdigit(codepoint)) {
        // the token is a float literal matching \.\d+[dD]?
        accumulate_codepoint(state, codepoint);
        return lex_float(state);
    }

    return construct_result(CORTECS_LEXER_TAG_DOT, state);
}

static cortecs_lexer_token_t lex_int_bad(lexer_state_t *state) {
    while (true) {
        UChar32 codepoint = current_codepoint(state);
        if (u_isalnum(codepoint) || codepoint == '_') {
            accumulate_codepoint(state, codepoint);
            continue;
        }

        break;
    }

    return construct_result(CORTECS_LEXER_TAG_BAD_INT, state);
}

static cortecs_lexer_token_t lex_int(lexer_state_t *state) {
    // [0-9]+([uU]?[bBsSlL])?
    while (true) {
        UChar32 codepoint = current_codepoint(state);
        if (codepoint == '.') {
            // the token is a float literal matching \d+\.\d*[dD]?
            accumulate_codepoint(state, codepoint);
            return lex_float(state);
        }

        if (u_isdigit(codepoint)) {
            accumulate_codepoint(state, codepoint);
            continue;
        }

        if (codepoint == 'b' || codepoint == 'B' || codepoint == 's' || codepoint == 'S' || codepoint == 'l' || codepoint == 'L') {
            accumulate_codepoint(state, codepoint);
            break;
        }

        if (codepoint == 'u' || codepoint == 'U') {
            accumulate_codepoint(state, codepoint);
            codepoint = current_codepoint(state);
            if (codepoint == 'b' || codepoint == 'B' || codepoint == 's' || codepoint == 'S' || codepoint == 'l' || codepoint == 'L') {
                accumulate_codepoint(state, codepoint);
                break;
            }

            return lex_int_bad(state);
        }

        break;
    }

    UChar32 codepoint = current_codepoint(state);
    if (u_isalnum(codepoint) || codepoint == '_') {
        accumulate_codepoint(state, codepoint);
        return lex_int_bad(state);
    }

    return construct_result(CORTECS_LEXER_TAG_INT, state);
}

// All ASCII characters are represented with the same numerical value
// in UTF-16 just with twice the space usage.
#define U8_LENGTH_OF_ASCII(kw) (sizeof(kw) - 1)
#define U8_IF_LENGTH U8_LENGTH_OF_ASCII("if")
#define U8_LET_LENGTH U8_LENGTH_OF_ASCII("let")
#define U8_RETURN_LENGTH U8_LENGTH_OF_ASCII("return")
#define U8_FUNCTION_LENGTH U8_LENGTH_OF_ASCII("function")

static bool check_keyword(lexer_state_t *state, const char *keyword) {
    int64_t end = utext_getNativeIndex(state->text);

    bool are_equal = true;
    utext_setNativeIndex(state->text, state->start);
    for (uint32_t i = 0; keyword[i] != 0; i++) {
        UChar32 codepoint = utext_current32(state->text);
        utext_next32(state->text);

        // ASCII characters are encoded with the same value in utf-32
        if (codepoint != keyword[i]) {
            are_equal = false;
            break;
        }
    }
    utext_setNativeIndex(state->text, end);

    return are_equal;
}

static bool is_first_codepoint_upper(lexer_state_t *state) {
    int64_t end = utext_getNativeIndex(state->text);
    utext_setNativeIndex(state->text, state->start);
    UChar32 codepoint = utext_current32(state->text);
    utext_setNativeIndex(state->text, end);
    return u_isupper(codepoint);
}

static cortecs_lexer_tag_t get_name_tag(lexer_state_t *state) {
    if (state->u8_length == U8_IF_LENGTH && check_keyword(state, "if")) {
        return CORTECS_LEXER_TAG_IF;
    }
    if (state->u8_length == U8_LET_LENGTH && check_keyword(state, "let")) {
        return CORTECS_LEXER_TAG_LET;
    }
    if (state->u8_length == U8_RETURN_LENGTH && check_keyword(state, "return")) {
        return CORTECS_LEXER_TAG_RETURN;
    }
    if (state->u8_length == U8_FUNCTION_LENGTH && check_keyword(state, "function")) {
        return CORTECS_LEXER_TAG_FUNCTION;
    }
    if (is_first_codepoint_upper(state)) {
        return CORTECS_LEXER_TAG_TYPE;
    }
    return CORTECS_LEXER_TAG_NAME;
}

static cortecs_lexer_token_t lex_name(lexer_state_t *state) {
    // [a-zA-Z][a-zA-Z0-9_]*
    while (true) {
        UChar32 codepoint = current_codepoint(state);
        if (u_isalnum(codepoint) || codepoint == '_') {
            accumulate_codepoint(state, codepoint);
            continue;
        }

        break;
    }

    return construct_result(get_name_tag(state), state);
}

static bool is_space(UChar32 codepoint) {
    return codepoint == ' ' || codepoint == '\t' || codepoint == '\r' ||
           codepoint == '\f' || codepoint == '\v';
}

static cortecs_lexer_token_t lex_whitespace(lexer_state_t *state) {
    // [\ \t\r\f\v]+
    while (true) {
        UChar32 codepoint = current_codepoint(state);
        if (is_space(codepoint)) {
            accumulate_codepoint(state, codepoint);
            continue;
        }

        break;
    }

    return construct_result(CORTECS_LEXER_TAG_SPACE, state);
}

static bool is_operator(UChar32 codepoint) {
    return codepoint == '!' || codepoint == '#' || codepoint == '$' ||
           codepoint == '%' || codepoint == '&' || codepoint == '*' ||
           codepoint == '+' || codepoint == '-' || codepoint == '/' ||
           codepoint == '<' || codepoint == '=' || codepoint == '>' ||
           codepoint == '?' || codepoint == '@' || codepoint == '\\' ||
           codepoint == '^' || codepoint == '|' || codepoint == '~';
}

static cortecs_lexer_token_t lex_operator(lexer_state_t *state) {
    // [\ \t\r\f\v]+
    while (true) {
        UChar32 codepoint = current_codepoint(state);
        if (is_operator(codepoint)) {
            accumulate_codepoint(state, codepoint);
            continue;
        }

        break;
    }

    return construct_result(CORTECS_LEXER_TAG_OPERATOR, state);
}

static bool is_invalid(UChar32 codepoint) {
    if (codepoint == '.' || codepoint == '(' || codepoint == ')' ||
        codepoint == '{' || codepoint == '}' || codepoint == '[' ||
        codepoint == ']' || codepoint == '\'' || codepoint == '"' ||
        codepoint == '`' || codepoint == ',' || codepoint == ':' ||
        codepoint == ';' || codepoint == '_' || codepoint == '\n' || codepoint == 0) {
        return false;
    }

    if (u_isalpha(codepoint)) {
        return false;
    }

    if (u_isdigit(codepoint)) {
        return false;
    }

    if (is_space(codepoint)) {
        return false;
    }

    if (is_operator(codepoint)) {
        return false;
    }

    return true;
}

static cortecs_lexer_token_t lex_invalid(lexer_state_t *state) {
    while (true) {
        UChar32 codepoint = current_codepoint(state);
        if (is_invalid(codepoint)) {
            accumulate_codepoint(state, codepoint);
            continue;
        }

        break;
    }

    return construct_result(CORTECS_LEXER_TAG_INVALID, state);
}

cortecs_lexer_token_t cortecs_lexer_next(UText *text) {
    if (text == NULL) {
        return (cortecs_lexer_token_t){
            .tag = CORTECS_LEXER_TAG_INVALID,
            .text = NULL,
            .span = {
                .lines = 0,
                .columns = 0,
            },
        };
    }

    lexer_state_t state = {
        .text = text,
        .start = utext_getNativeIndex(text),
        .u8_length = 0,
        .num_codepoints = 0,
        .span = {
            .lines = 0,
            .columns = 0,
        },
    };

    UChar32 codepoint = utext_current32(text);
    if (codepoint == U_SENTINEL) {
        return (cortecs_lexer_token_t){
            .tag = CORTECS_LEXER_TAG_INVALID,
            .text = NULL,
            .span = {
                .lines = 0,
                .columns = 0,
            },
        };
    }

    accumulate_codepoint(&state, codepoint);
    switch (codepoint) {
        case '.': {
            return lex_dot(&state);
        }
        case '\n': {
            state.span.columns = 0;
            state.span.lines++;
            return construct_result(CORTECS_LEXER_TAG_NEW_LINE, &state);
        }
        case '(': {
            return construct_result(CORTECS_LEXER_TAG_OPEN_PAREN, &state);
        }
        case ')': {
            return construct_result(CORTECS_LEXER_TAG_CLOSE_PAREN, &state);
        }
        case '{': {
            return construct_result(CORTECS_LEXER_TAG_OPEN_CURLY, &state);
        }
        case '}': {
            return construct_result(CORTECS_LEXER_TAG_CLOSE_CURLY, &state);
        }
        case '[': {
            return construct_result(CORTECS_LEXER_TAG_OPEN_SQUARE, &state);
        }
        case ']': {
            return construct_result(CORTECS_LEXER_TAG_CLOSE_SQUARE, &state);
        }
        case '\'': {
            return construct_result(CORTECS_LEXER_TAG_SINGLE_QUOTE, &state);
        }
        case '"': {
            return construct_result(CORTECS_LEXER_TAG_DOUBLE_QUOTE, &state);
        }
        case '`': {
            return construct_result(CORTECS_LEXER_TAG_BACK_QUOTE, &state);
        }
        case ',': {
            return construct_result(CORTECS_LEXER_TAG_COMMA, &state);
        }
        case ':': {
            return construct_result(CORTECS_LEXER_TAG_COLON, &state);
        }
        case ';': {
            return construct_result(CORTECS_LEXER_TAG_SEMICOLON, &state);
        }
        default: {
            if (u_isalpha(codepoint) || codepoint == '_') {
                return lex_name(&state);
            }

            if (u_isdigit(codepoint)) {
                return lex_int(&state);
            }

            if (is_space(codepoint)) {
                return lex_whitespace(&state);
            }

            if (is_operator(codepoint)) {
                return lex_operator(&state);
            }

            return lex_invalid(&state);
        }
    }
}