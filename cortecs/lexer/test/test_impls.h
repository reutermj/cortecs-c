#ifndef CORTECS_LEXER_TEST_TEST_IMPLS_H
#define CORTECS_LEXER_TEST_TEST_IMPLS_H

#include <stdbool.h>
#include <stdint.h>
#include <tokens.h>

typedef struct {
    uint32_t state;
    uint32_t index;
    uint32_t length;
} cortecs_lexer_test_state_t;

typedef struct {
    char next_char;
    uint32_t next_state;
} cortecs_lexer_test_result_t;

typedef struct {
    cortecs_lexer_test_result_t (*next)(cortecs_lexer_test_state_t, uint32_t);
    uint32_t (*state_max_entropy)(uint32_t);
    bool (*should_skip_token)(char *, uint32_t);
    cortecs_lexer_tag_t tag;
    uint32_t min_length;
} cortecs_lexer_test_config_t;

typedef struct {
    cortecs_lexer_test_config_t *configs;
    uint32_t **valid_next_token;
    uint32_t *lengths;
} cortecs_lexer_test_fuzz_config_t;

uint32_t cortecs_lexer_test(char *in, uint32_t offset, char *gold, cortecs_lexer_tag_t tag);
void cortecs_lexer_test_fuzz(cortecs_lexer_test_config_t config);
void cortecs_lexer_test_fuzz_multi(cortecs_lexer_test_fuzz_config_t config);
void cortecs_lexer_test_exhaustive(cortecs_lexer_test_config_t config);
bool cortecs_lexer_test_never_skip(char *, uint32_t);

#endif