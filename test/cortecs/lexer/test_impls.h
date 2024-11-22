#ifndef CORTECS_LEXER_TEST_TEST_IMPLS_H
#define CORTECS_LEXER_TEST_TEST_IMPLS_H

#include <cortecs/string.h>
#include <cortecs/tokens.h>
#include <stdbool.h>
#include <stdint.h>
#include <unicode/utext.h>

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
    bool (*should_skip_token)(const char *, uint32_t);
    cortecs_lexer_tag_t tag;
    uint32_t min_length;
    uint32_t max_length;
} cortecs_lexer_test_config_t;

typedef struct {
    cortecs_lexer_test_config_t *configs;
    bool **transition_to;
    uint32_t num_configs;
} cortecs_lexer_test_multi_config_t;

void cortecs_lexer_test(UText *text, CN(Cortecs, String) gold, cortecs_lexer_tag_t tag);
void cortecs_lexer_test_fuzz(cortecs_lexer_test_config_t config);
void cortecs_lexer_test_fuzz_multi(cortecs_lexer_test_multi_config_t config);
void cortecs_lexer_test_exhaustive(cortecs_lexer_test_config_t config);
void cortecs_lexer_test_exhaustive_two_token(cortecs_lexer_test_multi_config_t config);
bool cortecs_lexer_test_never_skip(const char *, uint32_t);

#endif