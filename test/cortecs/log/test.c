#include <cortecs/gc.h>
#include <cortecs/log.h>
#include <cortecs/string.h>
#include <cortecs/world.h>
#include <flecs.h>
#include <stdlib.h>
#include <unity.h>

void test_open_log() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
    CN(Cortecs, Log, init)();

    ecs_defer_begin(world);

    CN(Cortecs, String) path = CN(Cortecs, String, new)("./test_open_log.log");
    CN(Cortecs, Log, open)(path);

    ecs_defer_end(world);

    cortecs_world_cleanup();
}

void test_write_one_message() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
    CN(Cortecs, Log, init)();

    ecs_defer_begin(world);

    CN(Cortecs, String) path = CN(Cortecs, String, new)("./test_write_one_message.log");
    CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) log_stream = CN(Cortecs, Log, open)(path);

    cJSON *hello_world = cJSON_CreateObject();
    cJSON_AddStringToObject(hello_world, "message", "hello world");
    CN(Cortecs, Log, write)(log_stream, hello_world);
    cJSON_Delete(hello_world);

    ecs_defer_end(world);

    // cJSON *hi_there = cJSON_CreateObject();
    // cJSON_AddStringToObject(hi_there, "message", "hi there");
    // CN(Cortecs, Log, write)(log_stream, hi_there);
    // cJSON_Delete(hi_there);

    FILE *file = fopen("./test_write_one_message.log", "r");
    char line[1024];
    fgets(line, sizeof(line), file);
    cJSON *json = cJSON_Parse(line);
    cJSON *message = cJSON_GetObjectItem(json, "message");
    TEST_ASSERT_EQUAL_STRING("hello world", message->valuestring);

    cortecs_world_cleanup();
}

void test_write_two_messages() {
    cortecs_world_init();
    cortecs_finalizer_init();
    cortecs_gc_init(NULL);
    CN(Cortecs, Log, init)();

    ecs_defer_begin(world);

    CN(Cortecs, String) path = CN(Cortecs, String, new)("./test_write_two_messages.log");
    CN(Cortecs, Ptr, CT(CN(Cortecs, Log))) log_stream = CN(Cortecs, Log, open)(path);

    cJSON *hello_world = cJSON_CreateObject();
    cJSON_AddStringToObject(hello_world, "message", "hello world");
    CN(Cortecs, Log, write)(log_stream, hello_world);
    cJSON_Delete(hello_world);

    cJSON *hi_there = cJSON_CreateObject();
    cJSON_AddStringToObject(hi_there, "message", "hi there");
    CN(Cortecs, Log, write)(log_stream, hi_there);
    cJSON_Delete(hi_there);

    ecs_defer_end(world);

    FILE *file = fopen("./test_write_two_messages.log", "r");
    char line[1024];

    fgets(line, sizeof(line), file);
    printf("line %s", line);
    cJSON *hello_world_out = cJSON_Parse(line);
    cJSON *hello_world_message = cJSON_GetObjectItem(hello_world_out, "message");
    TEST_ASSERT_EQUAL_STRING("hello world", hello_world_message->valuestring);
    cJSON_Delete(hello_world_out);

    fgets(line, sizeof(line), file);
    printf("line %s", line);
    cJSON *hi_there_out = cJSON_Parse(line);
    cJSON *hi_there_message = cJSON_GetObjectItem(hi_there_out, "message");
    TEST_ASSERT_EQUAL_STRING("hi there", hi_there_message->valuestring);
    cJSON_Delete(hi_there_out);

    cortecs_world_cleanup();
}

int main() {
    UNITY_BEGIN();

    RUN_TEST(test_open_log);
    RUN_TEST(test_write_one_message);
    RUN_TEST(test_write_two_messages);

    return UNITY_END();
}

void setUp() {
}

void tearDown() {
    // required for unity
}