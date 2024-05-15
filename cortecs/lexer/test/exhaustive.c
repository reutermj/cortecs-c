#include "exhaustive.h"

#include <stdlib.h>

#include "util.h"

typedef struct {
    char *in;
    char *gold;
    int length;
    int offset;
} cortecs_lexer_exhaustive_state_t;

void lexer_test_gen(cortecs_lexer_exhaustive_config_t config, cortecs_lexer_exhaustive_state_t state, int index) {
    if (index == state.length) {
        if (state.length == 2 && strncmp(state.gold, "if", 2) == 0) {
            return;
        }

        if (state.length == 3 && strncmp(state.gold, "let", 3) == 0) {
            return;
        }

        if (state.length == 6 && strncmp(state.gold, "return", 6) == 0) {
            return;
        }

        if (state.length == 8 && strncmp(state.gold, "function", 8) == 0) {
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
        lexer_test_gen(config, state, index + 1);
    }
}

void cortecs_lexer_exhaustive_test(cortecs_lexer_exhaustive_config_t config) {
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
            lexer_test_gen(config, state, 0);
        }
    }
    free(state.in);
    free(state.gold);
}