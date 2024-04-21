#ifndef CORTECS_PARSER_TOKENIZER_H
#define CORTECS_PARSER_TOKENIZER_H

#include <tokens.h>
#include <stdint.h>

typedef struct {
    cortecs_token_t token;
    uint32_t start;
} cortecs_tokenizer_result_t;

cortecs_tokenizer_result_t cortecs_tokenizer_next(char *text, uint32_t start);

#endif