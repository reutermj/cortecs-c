#ifndef CORTECS_LEXER_TEST_UTIL_H
#define CORTECS_LEXER_TEST_UTIL_H

#include <stdint.h>
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
} cortecs_lexer_test_config_t;

bool cortecs_lexer_test_never_skip(char *, uint32_t);

void cortecs_lexer_test(char *in, uint32_t offset, char *gold, cortecs_lexer_tag_t tag);

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