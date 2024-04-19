#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <tokenizer.h>

cortecs_tokenizer_result_t cortecs_tokenizer_result(cortecs_token_tag_t tag, char *text, uint32_t start, uint32_t end) {
    char *token = (char *)calloc(end - start + 1, sizeof(char));
    memcpy(token, &text[start], end - start);

    return (cortecs_tokenizer_result_t){
        .start = end,
        .token = {
            .tag = tag,
            .text = token,
        },
    };
}

cortecs_tokenizer_result_t cortecs_tokenizer_next_float(char *text, uint32_t start, uint32_t end) {
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (!isdigit(c)) {
            break;
        }

        end++;
    }

    return cortecs_tokenizer_result(CORTECS_TOKEN_INT, text, start, end);
}

cortecs_tokenizer_result_t cortecs_tokenizer_next_int(char *text, uint32_t start) {
    uint32_t end = start + 1;
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (c == '.') {
            return cortecs_tokenizer_next_float(text, start, end + 1);
        }

        if (!isdigit(c)) {
            break;
        }

        end++;
    }

    return cortecs_tokenizer_result(CORTECS_TOKEN_INT, text, start, end);
}

cortecs_tokenizer_result_t cortecs_tokenizer_next_name(char *text, uint32_t start) {
    //[a-zA-Z_]+
    uint32_t end = start + 1;
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (isalnum(c) || c == '_') {
            end++;
            continue;
        }

        break;
    }

    uint32_t len = end - start;
    cortecs_token_tag_t tag;
    if (strncmp(&text[start], "function", len) == 0) {
        tag = CORTECS_TOKEN_FUNCTION;
    } else if (strncmp(&text[start], "let", len) == 0) {
        tag = CORTECS_TOKEN_LET;
    } else if (strncmp(&text[start], "if", len) == 0) {
        tag = CORTECS_TOKEN_IF;
    } else if (strncmp(&text[start], "return", len) == 0) {
        tag = CORTECS_TOKEN_RETURN;
    } else if (isupper(text[start])) {
        tag = CORTECS_TOKEN_TYPE;
    } else {
        tag = CORTECS_TOKEN_NAME;
    }

    return cortecs_tokenizer_result(tag, text, start, end);
}

cortecs_tokenizer_result_t cortecs_tokenizer_next_invalid(char *text, uint32_t start) {
    uint32_t end = start + 1;
    while (true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if (isdigit(c)) {
            break;
        }

        end++;
    }

    return cortecs_tokenizer_result(CORTECS_TOKEN_INVALID, text, start, end);
}

cortecs_tokenizer_result_t cortecs_tokenizer_next(char *text, uint32_t start) {
    char c = text[start];
    if (c == 0) {
        return cortecs_tokenizer_result(CORTECS_TOKEN_INVALID, "", 0, 0);
    }

    if (isalpha(c)) {
        return cortecs_tokenizer_next_name(text, start);
    }

    if (isdigit(c)) {
        return cortecs_tokenizer_next_int(text, start);
    }

    if (c == '.') {
        return cortecs_tokenizer_next_float(text, start, start + 1);
    }

    return cortecs_tokenizer_next_invalid(text, start);
}