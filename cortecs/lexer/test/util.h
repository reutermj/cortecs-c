#ifndef CORTECS_LEXER_TEST_UTIL_H
#define CORTECS_LEXER_TEST_UTIL_H

#include <stdint.h>

#define CORTECS_LEXER_SPACE_CHAR_MAX 5
char cortecs_lexer_space_char(uint32_t i);

#define CORTECS_LEXER_NAME_TYPE_FINALIZER_CHAR_MAX (CORTECS_LEXER_SPACE_CHAR_MAX + 1 + 1)
char cortecs_lexer_name_type_finalizer_char(uint32_t i);

#define CORTECS_LEXER_NAME_FIRST_CHAR_MAX 27
#define CORTECS_LEXER_NAME_VALID_CHAR_MAX 53
char cortecs_lexer_name_first_char(uint32_t i);
char cortecs_lexer_name_valid_char(uint32_t i);

#define CORTECS_LEXER_TYPE_FIRST_CHAR_MAX 26
#define CORTECS_LEXER_TYPE_VALID_CHAR_MAX 53
char cortecs_lexer_type_first_char(uint32_t i);
char cortecs_lexer_type_valid_char(uint32_t i);

#endif