#include <assert.h>
#include <stdbool.h>
#include <tokens.h>

const char *cortecs_lexer_tag_to_string(cortecs_lexer_tag_t tag) {
    switch (tag) {
        case CORTECS_LEXER_TAG_NAME:
            return "name";
        case CORTECS_LEXER_TAG_TYPE:
            return "type";
        case CORTECS_LEXER_TAG_INT:
            return "int";
        case CORTECS_LEXER_TAG_SPACE:
            return "space";
        case CORTECS_LEXER_TAG_FUNCTION:
            return "function";
        case CORTECS_LEXER_TAG_LET:
            return "let";
        case CORTECS_LEXER_TAG_RETURN:
            return "return";
        case CORTECS_LEXER_TAG_IF:
            return "if";
        case CORTECS_LEXER_TAG_INVALID:
            return "invalid";
    }
    assert(false);
}