#include "fuzz.h"

#include <span.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <tokens.h>

#include "util.h"

cortecs_lexer_token_t cortecs_lexer_fuzz_name() {
    uint32_t length = rand() % 256 + 1;
    char *text = calloc(length + 1, sizeof(char));

    while (true) {
        text[0] = cortecs_lexer_name_first_char(rand());
        for (int i = 1; i < length; i++) {
            text[i] = cortecs_lexer_name_valid_char(rand());
        }

        if (strncmp(text, "function", length) == 0 ||
            strncmp(text, "let", length) == 0 ||
            strncmp(text, "if", length) == 0 ||
            strncmp(text, "return", length) == 0) {
            continue;
        }

        break;
    }

    return (cortecs_lexer_token_t){
        .tag = CORTECS_LEXER_TAG_NAME,
        .text = text,
        .span = (cortecs_span_t){
            .columns = length,
            .lines = 0,
        },
    };
}

cortecs_lexer_token_t cortecs_lexer_fuzz_type() {
    uint32_t length = rand() % 256 + 1;
    char *text = calloc(length + 1, sizeof(char));
    text[0] = cortecs_lexer_type_first_char(rand());
    for (int i = 1; i < length; i++) {
        text[i] = cortecs_lexer_type_valid_char(rand());
    }

    return (cortecs_lexer_token_t){
        .tag = CORTECS_LEXER_TAG_TYPE,
        .text = text,
        .span = (cortecs_span_t){
            .columns = length,
            .lines = 0,
        },
    };
}

cortecs_lexer_token_t cortecs_lexer_fuzz_space() {
    uint32_t length = rand() % 256 + 1;

    char *text = calloc(length + 1, sizeof(char));
    for (int i = 0; i < length; i++) {
        text[i] = cortecs_lexer_space_char(rand());
    }

    return (cortecs_lexer_token_t){
        .tag = CORTECS_LEXER_TAG_SPACE,
        .text = text,
        .span = (cortecs_span_t){
            .columns = length,
            .lines = 0,
        },
    };
}