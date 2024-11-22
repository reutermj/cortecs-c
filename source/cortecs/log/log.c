#include <cortecs/gc.h>
#include <cortecs/log.h>

cortecs_finalizer_declare(CN(Cortecs, Log));
void cortecs_finalizer(CN(Cortecs, Log))(void *allocation) {
    CN(Cortecs, Log) log_stream = *(CN(Cortecs, Ptr, CT(CN(Cortecs, Log))))allocation;
    fclose(log_stream.log_file);
}

void CN(Cortecs, Log, init)() {
    cortecs_finalizer_register(CN(Cortecs, Log));
}

CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) CN(Cortecs, Log, open)(CN(Cortecs, String) path) {
    CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) log_stream = cortecs_gc_alloc(CN(Cortecs, Log));
    log_stream->log_file = fopen(path->content, "a+");
    if (log_stream->log_file == NULL) {
        // todo error
        return NULL;
    }

    return log_stream;
}

void CN(Cortecs, Log, write)(CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) log_stream, const cJSON *message) {
    char *message_string = cJSON_PrintUnformatted(message);
    if (message_string == NULL) {
        // todo error
        return;
    }

    fprintf(log_stream->log_file, "%s\n", message_string);
    cJSON_free(message_string);
}