#include <assert.h>
#include <ctype.h>
#include <lexer.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

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

static cortecs_lexer_result_t lex_float(char *text, uint32_t start, uint32_t end) {
    //([0-9]*\.[0-9]+) | ([0-9]+\.[0-9]*)
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (!isdigit(c)) {
            break;
        }

        end++;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_INT, text, start, end, span);
}

static cortecs_lexer_result_t lex_int(char *text, uint32_t start) {
    //[0-9]+
    uint32_t end = start + 1;
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (c == '.') {
            return lex_float(text, start, end + 1);
        }

        if (!isdigit(c)) {
            break;
        }

        end++;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_INT, text, start, end, span);
}

static cortecs_lexer_result_t lex_name(char *text, uint32_t start) {
    //[a-zA-Z][a-zA-Z0-9_]*
    uint32_t end = start + 1;
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (isalnum(c) || c == '_') {
            end++;
            continue;
        }

        break;
    }

    uint32_t len = end - start;
    cortecs_lexer_tag_t tag;

    if (len == 8 && strncmp(&text[start], "function", len) == 0) {
        tag = CORTECS_LEXER_TAG_FUNCTION;
    } else if (len == 3 && strncmp(&text[start], "let", len) == 0) {
        tag = CORTECS_LEXER_TAG_LET;
    } else if (len == 2 && strncmp(&text[start], "if", len) == 0) {
        tag = CORTECS_LEXER_TAG_IF;
    } else if (len == 6 && strncmp(&text[start], "return", len) == 0) {
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
    //[\ \t\r\f\v]+
    uint32_t end = start + 1;
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (!isspace(c)) {
            break;
        }

        if (c == '\n') {
            break;
        }

        end++;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_SPACE, text, start, end, span);
}

static cortecs_lexer_result_t lex_invalid(char *text, uint32_t start) {
    uint32_t end = start + 1;
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (isalnum(c)) {
            break;
        }

        if (isspace(c)) {
            break;
        }

        if (c == '.') {
            break;
        }

        end++;
    }

    cortecs_span_t span = {
        .lines = 0,
        .columns = end - start,
    };

    return construct_result(CORTECS_LEXER_TAG_INVALID, text, start, end, span);
}

cortecs_lexer_result_t cortecs_lexer_next(char *text, uint32_t start) {
    char c = text[start];
    if (c == 0) {
        cortecs_span_t span = {
            .lines = 0,
            .columns = 0,
        };
        return construct_result(CORTECS_LEXER_TAG_INVALID, "", 0, 0, span);
    }

    if (isalpha(c) || c == '_') {
        return lex_name(text, start);
    }

    if (isdigit(c)) {
        return lex_int(text, start);
    }

    if (c == '.') {
        return lex_float(text, start, start + 1);
    }

    if (c == '\n') {
        cortecs_span_t span = {
            .lines = 1,
            .columns = 0,
        };

        return construct_result(CORTECS_LEXER_TAG_NEW_LINE, text, start, start + 1, span);
    }

    if (isspace(c)) {
        return lex_whitespace(text, start);
    }

    return lex_invalid(text, start);
}