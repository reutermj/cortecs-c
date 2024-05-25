#include "test_impls.h"

#include <common.h>
#include <lexer.h>
#include <stdlib.h>
#include <tokens.h>
#include <unity.h>

uint32_t cortecs_lexer_test(char *in, uint32_t offset, char *gold, cortecs_lexer_tag_t tag) {
    cortecs_lexer_result_t result = cortecs_lexer_next(in, offset);

    int target_length = (int)strnlen(gold, 256);
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

    if (offset + target_length != result.start) {
        NOOP;
    }
    TEST_ASSERT_EQUAL_INT32(offset + target_length, result.start);

    if (gold_span.lines != result.token.span.lines) {
        NOOP;
    }
    TEST_ASSERT_EQUAL_INT32(gold_span.lines, result.token.span.lines);

    if (gold_span.columns != result.token.span.columns) {
        NOOP;
    }
    TEST_ASSERT_EQUAL_INT32(gold_span.columns, result.token.span.columns);

    if (result.token.tag != tag) {
        NOOP;
    }
    TEST_ASSERT_TRUE(result.token.tag == tag);

    int isDifferent = strncmp(gold, result.token.text, strlen(gold));
    if (isDifferent) {
        NOOP;
    }
    TEST_ASSERT_TRUE(!isDifferent);
    free(result.token.text);

    return result.start;
}

void cortecs_lexer_test_fuzz(cortecs_lexer_test_config_t config) {
    uint32_t start_length;
    if (config.min_length > 5) {
        start_length = config.min_length;
    } else {
        start_length = 5;
    }

    for (uint32_t length = start_length; length < 100; length++) {
        for (uint32_t offset = 0; offset < 100; offset++) {
            for (int times = 0; times < 100; times++) {
                char *in = calloc(offset + length + 1, sizeof(char));
                char *gold = calloc(length + 1, sizeof(char));
                while (true) {
                    cortecs_lexer_test_state_t state = {
                        .state = 0,
                        .length = length,
                    };
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
    char *gold;
    cortecs_lexer_tag_t tag;
} lexer_fuzz_case_t;

void cortecs_lexer_test_fuzz_multi(cortecs_lexer_test_fuzz_config_t config) {
    char *in = calloc(10150, sizeof(char));
    lexer_fuzz_case_t cases[10000];

    int num_cases = 0;
    uint32_t curr_config = 0;
    uint32_t offset = 0;
    while (offset < 10000) {
        cortecs_lexer_test_config_t next_config = config.configs[curr_config];
        uint32_t length = rand() % 100 + next_config.min_length;
        char *gold = calloc(length + 1, sizeof(char));
        while (true) {
            cortecs_lexer_test_state_t state = {
                .state = 0,
                .length = length,
            };
            for (uint32_t i = 0; i < length; i++) {
                state.index = i;
                cortecs_lexer_test_result_t result = next_config.next(state, rand());
                state.state = result.next_state;
                in[offset + i] = result.next_char;
                gold[i] = result.next_char;
            }

            if (next_config.should_skip_token(gold, length)) {
                state.state = 0;
                continue;
            }

            cases[num_cases].gold = gold;
            cases[num_cases].tag = next_config.tag;
            num_cases++;
            curr_config = config.valid_next_token[curr_config][rand() % config.lengths[curr_config]];

            break;
        }
        offset += length;
    }

    uint32_t start = 0;
    for (int i = 0; i < num_cases; i++) {
        lexer_fuzz_case_t gold = cases[i];
        start = cortecs_lexer_test(in, start, gold.gold, gold.tag);
    }

    free(in);
}

typedef struct {
    char *in;
    char *gold;
    uint32_t length;
    uint32_t offset;
    uint32_t state;
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

    uint32_t num_chars = config.state_max_entropy(0);
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
    uint32_t start_length;
    if (config.min_length > 1) {
        start_length = config.min_length;
    } else {
        start_length = 1;
    }

    // These constants were chosen to finish the test in a reasonable amount of time.
    const uint32_t max_offset = 5;
    const uint32_t max_length = 5;
    cortecs_lexer_exhaustive_state_stm_t state = {
        .in = calloc(max_length + max_offset + 1, sizeof(char)),
        .gold = calloc(max_length + 1, sizeof(char)),
    };
    for (uint32_t length = start_length; length <= max_length; length++) {
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