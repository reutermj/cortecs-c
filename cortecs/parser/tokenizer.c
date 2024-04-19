#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <tokenizer.h>
#include <string.h>
#include <stdlib.h>

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
    while(true) {
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
    while(true) {
        char c = text[end];
        if (c == 0) {
            break;
        }

        if(c == '.') {
            return cortecs_tokenizer_next_float(text, start, end + 1);
        }

        if (!isdigit(c)) {
            break;
        }

        end++;
    }

    return cortecs_tokenizer_result(CORTECS_TOKEN_INT, text, start, end);
}

cortecs_tokenizer_result_t cortecs_tokenizer_next_invalid(char *text, uint32_t start) {
    uint32_t end = start + 1;
    while(true) {
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

    if (isdigit(c)) {
        return cortecs_tokenizer_next_int(text, start);
    }

    if (c == '.') {
        return cortecs_tokenizer_next_float(text, start, start + 1);
    }

    return cortecs_tokenizer_next_invalid(text, start);
}