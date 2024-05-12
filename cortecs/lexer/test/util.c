#include "util.h"

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