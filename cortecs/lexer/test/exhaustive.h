#ifndef CORTECS_LEXER_TEST_EXHAUSTIVE_H
#define CORTECS_LEXER_TEST_EXHAUSTIVE_H

#include <stdbool.h>
#include <tokens.h>

typedef struct {
    char (*get_first_char)(uint32_t);
    int num_first_char;
    char (*get_other_chars)(uint32_t);
    int num_other_chars;
    char (*get_finalizer_char)(uint32_t);
    int num_finalizer_char;
    bool (*should_skip_token)(char *, uint32_t);
    cortecs_lexer_tag_t tag;
} cortecs_lexer_exhaustive_config_t;

void cortecs_lexer_exhaustive_test(cortecs_lexer_exhaustive_config_t config);
bool cortecs_lexer_exhaustive_never_skip(char *, uint32_t);

#endif