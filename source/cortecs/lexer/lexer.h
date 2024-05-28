#ifndef CORTECS_LEXER_LEXER_H
#define CORTECS_LEXER_LEXER_H

#include <stdint.h>
#include <tokens.h>

typedef struct {
    cortecs_lexer_token_t token;
    uint32_t start;
} cortecs_lexer_result_t;

cortecs_lexer_result_t cortecs_lexer_next(char *text, uint32_t start);

#endif