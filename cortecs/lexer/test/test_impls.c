#include "test_impls.h"

#include <common.h>
#include <lexer.h>
#include <stdlib.h>
#include <tokens.h>
#include <unity.h>

uint32_t cortecs_lexer_test(char *input, uint32_t offset, char *gold, cortecs_lexer_tag_t tag) {
    cortecs_lexer_result_t result = cortecs_lexer_next(input, offset);

    int target_length = (int)strnlen(gold, 256);
    cortecs_span_t gold_span = {
        .lines = 0,
        .columns = 0,
    };
    for (uint32_t i = 0;; i++) {
        char current_char = gold[i];
        if (current_char == 0) {
            break;
        }

        if (current_char == '\n') {
            gold_span.columns = 0;
            gold_span.lines = 1;
            break;
        }

        gold_span.columns++;
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
                char *input = calloc(offset + length + 1, sizeof(char));
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
                        input[offset + i] = result.next_char;
                        gold[i] = result.next_char;
                    }

                    if (config.should_skip_token(gold, length)) {
                        continue;
                    }

                    break;
                }
                input[offset + length] = 0;
                cortecs_lexer_test(input, offset, gold, config.tag);
                free(input);
                free(gold);
            }
        }
    }
}

typedef struct {
    char *gold;
    cortecs_lexer_tag_t tag;
} lexer_fuzz_case_t;

static uint32_t lexer_test_fuzz_case(cortecs_lexer_test_config_t config, char *input, uint32_t offset, lexer_fuzz_case_t *out) {
    uint32_t length = rand() % 100 + config.min_length;
    out->gold = calloc(length + 1, sizeof(char));
    while (true) {
        cortecs_lexer_test_state_t state = {
            .state = 0,
            .length = length,
        };
        for (uint32_t i = 0; i < length; i++) {
            state.index = i;
            cortecs_lexer_test_result_t result = config.next(state, rand());
            state.state = result.next_state;
            input[offset + i] = result.next_char;
            out->gold[i] = result.next_char;
        }

        if (config.should_skip_token(out->gold, length)) {
            state.state = 0;
            continue;
        }

        out->tag = config.tag;

        break;
    }
    return length;
}

void cortecs_lexer_test_fuzz_multi(cortecs_lexer_test_multi_config_t config) {
    char *input = calloc(10150, sizeof(char));
    lexer_fuzz_case_t cases[10000];

    int num_cases = 0;
    uint32_t curr_config = 0;
    uint32_t offset = 0;
    while (offset < 10000) {
        cortecs_lexer_test_config_t next_config = config.configs[curr_config];
        uint32_t length = lexer_test_fuzz_case(next_config, input, offset, &cases[num_cases]);
        num_cases++;
        offset += length;
        curr_config = config.valid_next_token[curr_config][rand() % config.lengths[curr_config]];
    }

    uint32_t start = 0;
    for (int i = 0; i < num_cases; i++) {
        lexer_fuzz_case_t gold = cases[i];
        start = cortecs_lexer_test(input, start, gold.gold, gold.tag);
        free(gold.gold);
    }

    free(input);
}

void cortecs_lexer_test_exhaustive_two_token(cortecs_lexer_test_multi_config_t config) {
    char input[250];
    lexer_fuzz_case_t cases[2];

    for (int times = 0; times < 1000; times++) {
        for (uint32_t i = 0; i < config.num_configs; i++) {
            cortecs_lexer_test_config_t first_config = config.configs[i];
            uint32_t first_length = lexer_test_fuzz_case(first_config, input, 0, &cases[0]);

            for (uint32_t j = 0; j < config.lengths[i]; j++) {
                uint32_t second_config_index = config.valid_next_token[i][j];
                cortecs_lexer_test_config_t second_config = config.configs[second_config_index];
                uint32_t second_length = lexer_test_fuzz_case(second_config, input, first_length, &cases[1]);
                input[first_length + second_length] = 0;

                uint32_t start = 0;
                for (int i = 0; i < 2; i++) {
                    lexer_fuzz_case_t gold = cases[i];
                    start = cortecs_lexer_test(input, start, gold.gold, gold.tag);
                }
                free(cases[1].gold);
            }
            free(cases[0].gold);
        }
    }
}

typedef struct {
    char *in;
    char *gold;
    uint32_t length;
    uint32_t offset;
    uint32_t state;
} cortecs_lexer_exhaustive_state_stm_t;

static void lexer_test_exhaustive_run(cortecs_lexer_test_config_t config, cortecs_lexer_exhaustive_state_stm_t state, uint32_t index) {
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

bool cortecs_lexer_test_never_skip(const char *string, uint32_t length) {
    UNUSED(string);
    UNUSED(length);
    return false;
}