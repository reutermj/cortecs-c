#include "tests.h"

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
                while (true) {
                    in[offset] = config.get_first_char(rand());
                    gold[0] = in[offset];
                    for (int i = 1; i < length; i++) {
                        in[offset + i] = config.get_other_chars(rand());
                        gold[i] = in[offset + i];
                    }

                    if (config.should_skip_token(gold, length)) {
                        continue;
                    }

                    break;
                }
                in[offset + length] = config.get_finalizer_char(rand());
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
} cortecs_lexer_exhaustive_state_t;

static void lexer_test_exhaustive_run(cortecs_lexer_test_config_t config, cortecs_lexer_exhaustive_state_t state, int index) {
    if (index == state.length) {
        if (config.should_skip_token(state.gold, state.length)) {
            return;
        }

        for (uint32_t i = 0; i < config.num_finalizer_char; i++) {
            state.in[state.offset + index] = config.get_finalizer_char(i);
            cortecs_lexer_test(state.in, state.offset, state.gold, config.tag);
        }

        return;
    }

    int num_chars;
    char (*get_char)(uint32_t);
    if (index == 0) {
        num_chars = config.num_first_char;
        get_char = config.get_first_char;
    } else {
        num_chars = config.num_other_chars;
        get_char = config.get_other_chars;
    }

    for (uint32_t i = 0; i < num_chars; i++) {
        char c = get_char(i);
        state.in[state.offset + index] = c;
        state.gold[index] = c;
        lexer_test_exhaustive_run(config, state, index + 1);
    }
}

void cortecs_lexer_test_exhaustive(cortecs_lexer_test_config_t config) {
    // These constants were chosen to finish the test in a reasonable amount of time.
    const uint32_t max_offset = 5;
    const uint32_t max_length = 5;
    cortecs_lexer_exhaustive_state_t state = {
        .in = calloc(max_length + max_offset + 1, sizeof(char)),
        .gold = calloc(max_length + 1, sizeof(char)),
    };
    for (uint32_t length = 1; length < max_length; length++) {
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