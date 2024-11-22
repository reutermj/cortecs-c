#ifndef LOG_LOG_H
#define LOG_LOG_H

#include <cJSON.h>
#include <cortecs/mangle.h>
#include <cortecs/string.h>
#include <stdio.h>

typedef struct CN(Cortecs, Log) {
    FILE *log_file;
} CN(Cortecs, Log);

#define TYPE_PARAM_T CN(Cortecs, Log)
#include <cortecs/array.template.h>
#include <cortecs/ptr.template.h>
#undef TYPE_PARAM_T

extern cortecs_finalizer_declare(CN(Cortecs, Log));

void CN(Cortecs, Log, init)();
CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) CN(Cortecs, Log, open)(CN(Cortecs, String) path);
void CN(Cortecs, Log, write)(CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) log_stream, const cJSON *message);

#endif