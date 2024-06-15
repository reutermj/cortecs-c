#ifndef CORTECS_LEXER_LEXER_H
#define CORTECS_LEXER_LEXER_H

#include <stdint.h>
#include <tokens.h>
#include <unicode/utext.h>

cortecs_lexer_token_t cortecs_lexer_next(UText *text);

#endif