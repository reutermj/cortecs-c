#ifndef CORTECS_LEXER_TOKENS_H
#define CORTECS_LEXER_TOKENS_H

#include <span.h>

typedef enum {
    CORTECS_LEXER_TAG_NAME,
    CORTECS_LEXER_TAG_TYPE,
    CORTECS_LEXER_TAG_INT,
    CORTECS_LEXER_TAG_FLOAT,
    CORTECS_LEXER_TAG_BAD_FLOAT,

    CORTECS_LEXER_TAG_SPACE,
    CORTECS_LEXER_TAG_NEW_LINE,

    CORTECS_LEXER_TAG_FUNCTION,
    CORTECS_LEXER_TAG_LET,
    CORTECS_LEXER_TAG_RETURN,
    CORTECS_LEXER_TAG_IF,

    CORTECS_LEXER_TAG_DOT,

    CORTECS_LEXER_TAG_INVALID,
} cortecs_lexer_tag_t;

typedef struct {
    cortecs_lexer_tag_t tag;
    cortecs_span_t span;
    char *text;
} cortecs_lexer_token_t;

const char *cortecs_lexer_tag_to_string(cortecs_lexer_tag_t tag);

#endif