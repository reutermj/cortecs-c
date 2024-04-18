#include <assert.h>
#include <ctype.h>
#include <stdbool.h>
#include <tokenizer.h>
#define loop while (true)

cortecs_tokenizer_result_t cortecs_tokenizer_result(cortecs_token_tag_t tag, sds text, uint32_t start, uint32_t end) {
    return (cortecs_tokenizer_result_t){
        .start = end,
        .token = {
            .tag = CORTECS_TOKEN_INT,
            .text = sdsnewlen(&text[start], end - start),
        },
    };
}

cortecs_tokenizer_result_t cortecs_tokenizer_next_int(sds text, uint32_t start) {
    uint32_t end = start + 1;
    loop {
        if (end == sdslen(text)) {
            break;
        }

        char c = text[end];
        // if(c == '.') {
        //     return cortecs_tokenizer_next_float(text, start, end);
        // }

        if (!isdigit(c)) {
            break;
        }
    }

    return cortecs_tokenizer_result(CORTECS_TOKEN_INT, text, start, end);
}

cortecs_tokenizer_result_t cortecs_tokenizer_next_invalid(sds text, uint32_t start) {
    uint32_t end = start + 1;
    loop {
        if (end == sdslen(text)) {
            break;
        }

        char c = text[end];
        if (isdigit(c)) {
            break;
        }
    }

    return cortecs_tokenizer_result(CORTECS_TOKEN_INVALID, text, start, end);
}

cortecs_tokenizer_result_t cortecs_tokenizer_next(sds text, uint32_t start) {
    assert(start < sdslen(text));

    if (isdigit(text[start])) {
        return cortecs_tokenizer_next_int(text, start);
    }

    return cortecs_tokenizer_next_invalid(text, start);
}