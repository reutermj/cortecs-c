#ifndef CORTECS_PARSER_TOKENS_H
#define CORTECS_PARSER_TOKENS_H

#include <stdint.h>

typedef struct {
    uint32_t lines;
    uint32_t columns;
} cortecs_span_t;

typedef enum {
    CORTECS_TOKEN_NAME,
    CORTECS_TOKEN_TYPE,
    CORTECS_TOKEN_INT,

    CORTECS_TOKEN_WHITESPACE,

    CORTECS_TOKEN_FUNCTION,
    CORTECS_TOKEN_LET,
    CORTECS_TOKEN_RETURN,
    CORTECS_TOKEN_IF,

    CORTECS_TOKEN_INVALID,
} cortecs_token_tag_t;

typedef struct {
    cortecs_token_tag_t tag;
    cortecs_span_t span;
    char *text;
} cortecs_token_t;

const char *cortecs_token_tag_to_string(cortecs_token_tag_t tag);

#endif