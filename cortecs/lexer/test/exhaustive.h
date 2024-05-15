#ifndef CORTECS_LEXER_TEST_EXHAUSTIVE_H
#define CORTECS_LEXER_TEST_EXHAUSTIVE_H

#include <tokens.h>

typedef struct {
    char (*get_first_char)(uint32_t);
    int num_first_char;
    char (*get_other_chars)(uint32_t);
    int num_other_chars;
    char (*get_finalizer_char)(uint32_t);
    int num_finalizer_char;
    cortecs_lexer_tag_t tag;
} cortecs_lexer_exhaustive_config_t;

void cortecs_lexer_exhaustive_test(cortecs_lexer_exhaustive_config_t config);

#endif