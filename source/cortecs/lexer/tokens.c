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
        case CORTECS_LEXER_TAG_BAD_INT:
            return "bad_int";
        case CORTECS_LEXER_TAG_FLOAT:
            return "float";
        case CORTECS_LEXER_TAG_BAD_FLOAT:
            return "bad_float";
        case CORTECS_LEXER_TAG_OPERATOR:
            return "operator";
        case CORTECS_LEXER_TAG_SPACE:
            return "space";
        case CORTECS_LEXER_TAG_NEW_LINE:
            return "new_line";
        case CORTECS_LEXER_TAG_FUNCTION:
            return "function";
        case CORTECS_LEXER_TAG_LET:
            return "let";
        case CORTECS_LEXER_TAG_RETURN:
            return "return";
        case CORTECS_LEXER_TAG_IF:
            return "if";
        case CORTECS_LEXER_TAG_DOT:
            return "dot";
        case CORTECS_LEXER_TAG_OPEN_PAREN:
            return "open_paren";
        case CORTECS_LEXER_TAG_CLOSE_PAREN:
            return "close_paren";
        case CORTECS_LEXER_TAG_OPEN_CURLY:
            return "open_curly";
        case CORTECS_LEXER_TAG_CLOSE_CURLY:
            return "close_curly";
        case CORTECS_LEXER_TAG_OPEN_SQUARE:
            return "open_square";
        case CORTECS_LEXER_TAG_CLOSE_SQUARE:
            return "close_square";
        case CORTECS_LEXER_TAG_INVALID:
            return "invalid";
    }
    return "unknown";
}