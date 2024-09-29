#ifndef LOG_LOG_H
#define LOG_LOG_H

#include <cJSON.h>
#include <cortecs/string.h>
#include <cortecs/type.h>
#include <stdio.h>

typedef struct cortecs_log_stream *cortecs_log_stream;
struct cortecs_log_stream {
    FILE *log_file;
};
cortecs_type_forward_declare(cortecs_log_stream);
cortecs_type_declare(cortecs_log_stream);

void cortecs_log_init();
cortecs_log_stream cortecs_log_open(cortecs_string path);
void cortecs_log_write(cortecs_log_stream log_stream, const cJSON *message);

#endif