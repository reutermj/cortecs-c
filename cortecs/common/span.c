#include <span.h>

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