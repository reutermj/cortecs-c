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

cortecs_span_t cortecs_span_of(CN(Cortecs, String) text) {
    if (text.content == NULL) {
        return (cortecs_span_t){
            .lines = 0,
            .columns = 0,
        };
    }

    cortecs_span_t out = {
        .lines = 0,
        .columns = 0,
    };

    uint32_t capacity = CN(Cortecs, String, capacity)(text);
    for (uint32_t i = 0; i < capacity; i++) {
        uint8_t current_char = text.content->elements[i];
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