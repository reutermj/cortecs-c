#include "test_impls.h"

#include <common.h>
#include <lexer.h>
#include <stdlib.h>
#include <unity.h>

void cortecs_lexer_test(char *in, uint32_t offset, char *gold, cortecs_lexer_tag_t tag) {
    cortecs_lexer_result_t result = cortecs_lexer_next(in, offset);

    int target_length = strlen(gold);
    cortecs_span_t gold_span = {
        .lines = 0,
        .columns = 0,
    };
    for (uint32_t i = 0;; i++) {
        char c = gold[i];
        if (c == 0) {
            break;
        }

        if (c == '\n') {
            gold_span.columns = 0;
            gold_span.lines = 1;
            break;
        } else {
            gold_span.columns++;
        }
    }

    TEST_ASSERT_EQUAL_INT32(offset + target_length, result.start);
    TEST_ASSERT_EQUAL_INT32(gold_span.lines, result.token.span.lines);
    TEST_ASSERT_EQUAL_INT32(gold_span.columns, result.token.span.columns);
    TEST_ASSERT_TRUE(result.token.tag == tag);
    TEST_ASSERT_TRUE(strncmp(gold, result.token.text, strlen(gold)) == 0);
    free(result.token.text);
}

void cortecs_lexer_test_fuzz(cortecs_lexer_test_config_t config) {
    for (uint32_t length = 5; length < 100; length++) {
        for (uint32_t offset = 0; offset < 100; offset++) {
            for (int times = 0; times < 100; times++) {
                char *in = calloc(offset + length + 1, sizeof(char));
                char *gold = calloc(length + 1, sizeof(char));
                cortecs_lexer_test_state_t state = {
                    .state = 0,
                    .length = length,
                };
                while (true) {
                    for (uint32_t i = 0; i < length; i++) {
                        state.index = i;
                        cortecs_lexer_test_result_t result = config.next(state, rand());
                        state.state = result.next_state;
                        in[offset + i] = result.next_char;
                        gold[i] = result.next_char;
                    }

                    if (config.should_skip_token(gold, length)) {
                        continue;
                    }

                    break;
                }
                in[offset + length] = 0;
                cortecs_lexer_test(in, offset, gold, config.tag);
                free(in);
                free(gold);
            }
        }
    }
}

typedef struct {
    char *in;
    char *gold;
    int length;
    int offset;
    int state;
} cortecs_lexer_exhaustive_state_stm_t;

static void lexer_test_exhaustive_run(cortecs_lexer_test_config_t config, cortecs_lexer_exhaustive_state_stm_t state, int index) {
    if (index == state.length) {
        if (config.should_skip_token(state.gold, state.length)) {
            return;
        }

        state.in[state.offset + index] = 0;
        state.gold[index] = 0;
        cortecs_lexer_test(state.in, state.offset, state.gold, config.tag);

        return;
    }

    cortecs_lexer_test_state_t test_state = {
        .state = state.state,
        .index = index,
        .length = state.length,
    };

    int num_chars = config.state_max_entropy(0);
    for (uint32_t i = 0; i < num_chars; i++) {
        cortecs_lexer_test_result_t result = config.next(test_state, i);
        state.in[state.offset + index] = result.next_char;
        state.gold[index] = result.next_char;

        cortecs_lexer_exhaustive_state_stm_t next_state = {
            .in = state.in,
            .gold = state.gold,
            .length = state.length,
            .offset = state.offset,
            .state = result.next_state,
        };
        lexer_test_exhaustive_run(config, next_state, index + 1);
    }
}

void cortecs_lexer_test_exhaustive(cortecs_lexer_test_config_t config) {
    // These constants were chosen to finish the test in a reasonable amount of time.
    const uint32_t max_offset = 5;
    const uint32_t max_length = 5;
    cortecs_lexer_exhaustive_state_stm_t state = {
        .in = calloc(max_length + max_offset + 1, sizeof(char)),
        .gold = calloc(max_length + 1, sizeof(char)),
    };
    for (uint32_t length = 1; length <= max_length; length++) {
        for (uint32_t offset = 0; offset <= max_offset; offset++) {
            state.length = length;
            state.offset = offset;
            lexer_test_exhaustive_run(config, state, 0);
        }
    }
    free(state.in);
    free(state.gold);
}

bool cortecs_lexer_test_never_skip(char *string, uint32_t length) {
    UNUSED(string);
    UNUSED(length);
    return false;
}