#include "util.h"

char cortecs_lexer_space_char(uint32_t i) {
    switch (i % CORTECS_LEXER_SPACE_CHAR_MAX) {
        case 0:
            return ' ';
        case 1:
            return '\t';
        case 2:
            return '\r';
        case 3:
            return '\v';
        default:
            return '\f';
    }
}

char cortecs_lexer_name_type_finalizer_char(uint32_t i) {
    uint32_t j = i % CORTECS_LEXER_NAME_TYPE_FINALIZER_CHAR_MAX;
    if (j < CORTECS_LEXER_SPACE_CHAR_MAX) {
        return cortecs_lexer_space_char(j);
    }

    j -= CORTECS_LEXER_SPACE_CHAR_MAX;

    if (j == 0) {
        return '\n';
    }

    return 0;
}

char cortecs_lexer_name_first_char(uint32_t i) {
    uint32_t j = i % CORTECS_LEXER_NAME_FIRST_CHAR_MAX;
    if (j < 26) {
        return 'a' + j;
    }

    return '_';
}

char cortecs_lexer_name_valid_char(uint32_t i) {
    uint32_t j = i % CORTECS_LEXER_NAME_VALID_CHAR_MAX;
    if (j < 26) {
        return 'a' + j;
    }

    if (j < 52) {
        return 'A' + (j - 26);
    }

    return '_';
}

char cortecs_lexer_type_first_char(uint32_t i) {
    return 'A' + i % CORTECS_LEXER_TYPE_FIRST_CHAR_MAX;
}

char cortecs_lexer_type_valid_char(uint32_t i) {
    uint32_t j = i % CORTECS_LEXER_TYPE_VALID_CHAR_MAX;
    if (j < 26) {
        return 'A' + j;
    }

    if (j < 52) {
        return 'a' + (j - 26);
    }

    return '_';
}