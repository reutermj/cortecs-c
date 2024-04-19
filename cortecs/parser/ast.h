#ifndef CORTECS_AST_AST_H
#define CORTECS_AST_AST_H

typedef enum {
    CORTECS_TOKEN_NAME,
    CORTECS_TOKEN_INT,
    CORTECS_TOKEN_WHITESPACE,
    CORTECS_TOKEN_NEWLINE,
    CORTECS_TOKEN_INVALID,
} cortecs_token_tag_t;

typedef struct {
    cortecs_token_tag_t tag;
    char *text;
} cortecs_token_t;

#endif