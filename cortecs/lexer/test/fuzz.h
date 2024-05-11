#ifndef CORTECS_LEXER_TEST_FUZZ_H
#define CORTECS_LEXER_TEST_FUZZ_H

#include <tokens.h>

void cortecs_lexer_fuzz_init();
cortecs_lexer_token_t cortecs_lexer_fuzz_name();

#endif