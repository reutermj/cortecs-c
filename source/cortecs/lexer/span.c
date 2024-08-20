#include <cortecs/span.h>

int cortecs_span_compare(cortecs_span_t left, cortecs_span_t right) {
    if (left.lines < right.lines) {
        return -1;
    }

    if (left.lines > right.lines) {
        return 1;
    }

    if (left.columns < right.columns) {
        return -1;
    }

    if (left.columns > right.columns) {
        return 1;
    }

    return 0;
}

cortecs_span_t cortecs_span_add(cortecs_span_t left, cortecs_span_t right) {
    if (right.lines != 0) {
        return (cortecs_span_t){
            .lines = left.lines + right.lines,
            .columns = right.columns,
        };
    }

    return (cortecs_span_t){
        .lines = left.lines,
        .columns = left.columns + right.columns,
    };
}

cortecs_span_t cortecs_span_of(cortecs_string text) {
    if (text == NULL) {
        return (cortecs_span_t){
            .lines = 0,
            .columns = 0,
        };
    }

    cortecs_span_t out = {
        .lines = 0,
        .columns = 0,
    };

    for (uint32_t i = 0; i < text->size; i++) {
        uint8_t current_char = text->content[i];
        if (current_char == 0) {
            break;
        }

        if (current_char == '\n') {
            out.columns = 0;
            out.lines++;
            continue;
        }

        out.columns++;
    }

    return out;
}