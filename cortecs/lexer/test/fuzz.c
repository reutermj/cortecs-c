#include "fuzz.h"

#include <span.h>
#include <stdint.h>
#include <stdlib.h>
#include <time.h>
#include <tokens.h>

// [_a-z] are valid first letters for names
#define NUM_VALID_NAME_CHARS_FIRST_LETTER (1 + 26)
// [_a-zA-Z0-9] are valid for names after the first letter
#define NUM_VALID_NAME_CHARS (NUM_VALID_NAME_CHARS_FIRST_LETTER + 26 + 10)
char valid_name_type_chars[NUM_VALID_NAME_CHARS];

// [A-Z] are valid first letters for types
#define NUM_VALID_TYPE_CHARS_FIRST_LETTER (26)
// [A-Z] are in position 27-53 in the array
#define NUM_VALID_TYPE_CHARS_FIRST_LETTER_OFFSET (26 + 1)

void cortecs_lexer_fuzz_init() {
    srand(time(NULL));

    int i = 0;
    valid_name_type_chars[i++] = '_';
    for (char c = 'a'; c <= 'z'; c++) {
        valid_name_type_chars[i++] = c;
    }
    for (char c = 'A'; c <= 'Z'; c++) {
        valid_name_type_chars[i++] = c;
    }
    for (char c = '0'; c <= '9'; c++) {
        valid_name_type_chars[i++] = c;
    }
}

cortecs_lexer_token_t cortecs_lexer_fuzz_name() {
    uint32_t length = rand() % 256 + 1;
    char *text = calloc(length + 1, sizeof(char));

    while (true) {
        text[0] = valid_name_type_chars[rand() % NUM_VALID_NAME_CHARS_FIRST_LETTER];
        for (int i = 1; i < length; i++) {
            text[i] = valid_name_type_chars[rand() % NUM_VALID_NAME_CHARS];
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
    text[0] = valid_name_type_chars[rand() % NUM_VALID_TYPE_CHARS_FIRST_LETTER + NUM_VALID_TYPE_CHARS_FIRST_LETTER_OFFSET];
    for (int i = 1; i < length; i++) {
        text[i] = valid_name_type_chars[rand() % NUM_VALID_NAME_CHARS];
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