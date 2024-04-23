#include <assert.h>
#include <stdbool.h>
#include <tokens.h>

const char *cortecs_token_tag_to_string(cortecs_token_tag_t tag) {
    switch (tag) {
        case CORTECS_TOKEN_NAME:
            return "name";
        case CORTECS_TOKEN_TYPE:
            return "type";
        case CORTECS_TOKEN_INT:
            return "int";
        case CORTECS_TOKEN_WHITESPACE:
            return "whitespace";
        case CORTECS_TOKEN_FUNCTION:
            return "function";
        case CORTECS_TOKEN_LET:
            return "let";
        case CORTECS_TOKEN_RETURN:
            return "return";
        case CORTECS_TOKEN_IF:
            return "if";
        case CORTECS_TOKEN_INVALID:
            return "invalid";
    }
    assert(false);
}