#include "test_impls.h"

#include <common.h>
#include <cortecs/lexer.h>
#include <cortecs/string.h>
#include <cortecs/tokens.h>
#include <cortecs/world.h>
#include <stdlib.h>
#include <unicode/urename.h>
#include <unicode/utypes.h>
#include <unity.h>

void cortecs_lexer_test(UText *text, char *gold, cortecs_lexer_tag_t tag) {
    ecs_defer_begin(world);
    cortecs_lexer_token_t out = cortecs_lexer_next(text);

    string_t gold_text = {
        .length = strlen(gold),
        .content = (uint8_t *)gold,
    };

    cortecs_span_t gold_span = cortecs_span_of(gold_text);

    if (gold_span.lines != out.span.lines) {
        NOOP;
    }
    TEST_ASSERT_EQUAL_INT32(gold_span.lines, out.span.lines);

    if (gold_span.columns != out.span.columns) {
        NOOP;
    }
    TEST_ASSERT_EQUAL_INT32(gold_span.columns, out.span.columns);

    if (out.tag != tag) {
        NOOP;
    }
    TEST_ASSERT_TRUE(out.tag == tag);

    int areEqual = string_equals(gold_text, out.text);
    if (!areEqual) {
        NOOP;
    }
    TEST_ASSERT_TRUE(areEqual);
    ecs_defer_end(world);
}

void cortecs_lexer_test_fuzz(cortecs_lexer_test_config_t config) {
    uint32_t start_length;
    if (config.min_length > 5) {
        start_length = config.min_length;
    } else {
        start_length = 5;
    }

    uint32_t max_length;
    if (config.max_length < 100) {
        max_length = config.max_length;
    } else {
        max_length = 100;
    }

    for (uint32_t length = start_length; length < max_length; length++) {
        for (int64_t offset = 0; offset < 100; offset++) {
            for (int times = 0; times < 100; times++) {
                int64_t input_length = offset + length + 1;
                char *input = calloc(input_length, sizeof(char));
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

                // not initializing status to U_ZERO_ERROR here caused utext_openUTF8 to return NULL
                UErrorCode status = U_ZERO_ERROR;
                UText *text = utext_openUTF8(NULL, input, input_length, &status);
                utext_setNativeIndex(text, offset);
                cortecs_lexer_test(text, gold, config.tag);
                utext_close(text);
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
    uint32_t max_length;
    if (config.max_length < 100) {
        max_length = config.max_length;
    } else {
        max_length = 100;
    }

    uint32_t length;
    if (max_length == config.min_length) {
        length = config.min_length;
    } else {
        length = (rand() % (max_length - config.min_length)) + config.min_length;
    }

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
        uint32_t fuel = rand() % config.num_configs;
        for (uint32_t i = 0; true; i = (i + 1) % config.num_configs) {
            if (config.transition_to[curr_config][i]) {
                if (fuel == 0) {
                    curr_config = i;
                    break;
                }
                fuel--;
            }
        }
    }

    UErrorCode status = U_ZERO_ERROR;
    UText *text = utext_openUTF8(NULL, input, offset + 1, &status);
    for (int i = 0; i < num_cases; i++) {
        lexer_fuzz_case_t gold = cases[i];
        cortecs_lexer_test(text, gold.gold, gold.tag);
        free(gold.gold);
    }
    utext_close(text);

    free(input);
}

void cortecs_lexer_test_exhaustive_two_token(cortecs_lexer_test_multi_config_t config) {
    char input[250];
    lexer_fuzz_case_t cases[2];

    for (int times = 0; times < 1000; times++) {
        for (uint32_t i = 0; i < config.num_configs; i++) {
            cortecs_lexer_test_config_t first_config = config.configs[i];
            uint32_t first_length = lexer_test_fuzz_case(first_config, input, 0, &cases[0]);

            for (uint32_t j = 0; j < config.num_configs; j++) {
                if (!config.transition_to[i][j]) {
                    continue;
                }
                cortecs_lexer_test_config_t second_config = config.configs[j];
                uint32_t second_length = lexer_test_fuzz_case(second_config, input, first_length, &cases[1]);
                input[first_length + second_length] = 0;

                UErrorCode status = U_ZERO_ERROR;
                UText *text = utext_openUTF8(NULL, input, first_length + second_length + 1, &status);
                for (int i = 0; i < 2; i++) {
                    lexer_fuzz_case_t gold = cases[i];
                    cortecs_lexer_test(text, gold.gold, gold.tag);
                }
                utext_close(text);
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
        UErrorCode status = U_ZERO_ERROR;
        UText *text = utext_openUTF8(NULL, state.in, state.offset + state.length + 1, &status);
        utext_setNativeIndex(text, state.offset);
        cortecs_lexer_test(text, state.gold, config.tag);
        utext_close(text);

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

    uint32_t max_length;
    if (config.max_length < 5 || config.min_length > 5) {
        max_length = config.max_length;
    } else {
        max_length = 5;
    }

    // These constants were chosen to finish the test in a reasonable amount of time.
    const uint32_t max_offset = 5;
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