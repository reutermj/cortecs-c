#include <cortecs/gc.h>
#include <cortecs/log.h>

cortecs_type_define(cortecs_log_stream);

void cortecs_finalizer(cortecs_log_stream)(void *allocation) {
    cortecs_log_stream log_stream = allocation;
    fclose(log_stream->log_file);
}

void cortecs_log_init() {
    cortecs_type_register_finalizer(cortecs_log_stream);
}

cortecs_log_stream cortecs_log_open(cortecs_string path) {
    cortecs_log_stream log_stream = cortecs_gc_alloc(cortecs_type_arg(cortecs_log_stream));
    log_stream->log_file = fopen(path->content, "a+");
    if (log_stream->log_file == NULL) {
        // todo error
        return NULL;
    }

    return log_stream;
}

void cortecs_log_write(cortecs_log_stream log_stream, const cJSON *message) {
    char *message_string = cJSON_PrintUnformatted(message);
    if (message_string == NULL) {
        // todo error
        return;
    }

    fprintf(log_stream->log_file, "%s\n", message_string);
    cJSON_free(message_string);
}