#include <assert.h>
#include <cJSON.h>
#include <common.h>
#include <cortecs/gc.h>
#include <cortecs/lsp.h>
#include <cortecs/string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

cortecs_type_define(cortecs_lsp_any);

#define CORTECS_LSP_SUCCESS               \
    (cortecs_lsp_parse_error_t) {         \
        .tag = CORTECS_LSP_PARSE_SUCCESS, \
        .message = NULL,                  \
    }

static cortecs_lsp_any accept_any(const cJSON *field);

static cortecs_lsp_parse_error_t find_field(const cJSON *json, const char *field_name, bool is_optional, const cJSON **out) {
    const cJSON *field = cJSON_GetObjectItemCaseSensitive(json, field_name);
    if (field == NULL) {
        if (is_optional) {
            return (cortecs_lsp_parse_error_t){
                .tag = CORTECS_LSP_PARSE_SUCCESS_NOT_FOUND,
                .message = NULL,
            };
        }

        return (cortecs_lsp_parse_error_t){
            .tag = CORTECS_LSP_PARSE_MISSING_REQUIRED_FIELD,
            .message = cortecs_string_new("%s expected but not found", field_name),
        };
    }

    *out = field;

    return CORTECS_LSP_SUCCESS;
}

cortecs_lsp_parse_error_t incorrect_type_message(const cJSON *field, const char *type_string) {
    char *field_str = cJSON_Print(field);
    cortecs_string error_message = cortecs_string_new(
        "%s expected to be %s, found %s",
        field->string,
        type_string,
        field_str
    );
    free(field_str);

    return (cortecs_lsp_parse_error_t){
        .tag = CORTECS_LSP_PARSE_TYPE_ERROR,
        .message = error_message,
    };
}

static bool is_string(const cJSON *field) {
    return cJSON_IsString(field) && field->valuestring != NULL;
}

static cortecs_lsp_parse_error_t expect_string(const cJSON *field) {
    if (is_string(field)) {
        return CORTECS_LSP_SUCCESS;
    }

    return incorrect_type_message(field, "string");
}

static cortecs_string accept_string(const cJSON *field) {
    assert(is_string(field));
    return cortecs_string_new("%s", field->valuestring);
}

static bool is_object(const cJSON *field) {
    return cJSON_IsObject(field);
}

// something is complaining about these being unused. They'll be used at some point
// static lsp_parse_error_t expect_object(const cJSON *field) {
//     if (is_object(field)) {
//         return CORTECS_LSP_SUCCESS;
//     }

//     return incorrect_type_message(field, "object");
// }

static cortecs_lsp_object accept_object(const cJSON *field) {
    assert(is_object(field));

    if (field->child == NULL) {
        // object is empty: {}
        return (cortecs_lsp_object){
            .field_names = NULL,
            .field_values = NULL,
        };
    }

    // count number of fields
    uint32_t size = 0;
    for (cJSON *current = field->child; current != NULL; current = current->next) {
        size++;
    }

    cortecs_array(cortecs_string) names = cortecs_gc_alloc_array(cortecs_type_arg(cortecs_string), size);
    cortecs_array(cortecs_lsp_any) values = cortecs_gc_alloc_array(cortecs_type_arg(cortecs_lsp_any), size);

    // read all fields into the arrays
    uint32_t index = 0;
    for (cJSON *current = field->child; current != NULL; current = current->next) {
        names->elements[index] = cortecs_string_new("%s", current->string);
        values->elements[index] = accept_any(current);
    }

    return (cortecs_lsp_object){
        .field_names = names,
        .field_values = values,
    };
}

static bool is_array(const cJSON *field) {
    return cJSON_IsArray(field);
}

// something is complaining about these being unused. They'll be used at some point
// static cortecs_lsp_parse_error_t expect_array(const cJSON *field) {
//     if (is_array(field)) {
//         return CORTECS_LSP_SUCCESS;
//     }

//     return incorrect_type_message(field, "array");
// }

static cortecs_array(cortecs_lsp_any) accept_array(const cJSON *field) {
    assert(cJSON_IsArray(field));

    if (field->child == NULL) {
        // array is empty: []
        return cortecs_gc_alloc_array(cortecs_type_arg(cortecs_lsp_any), 0);
    }

    cJSON *current = field->child;

    // count size of array
    uint32_t size = 0;
    while (current != NULL) {
        size++;
        current = current->next;
    }

    cortecs_array(cortecs_lsp_any) elements = cortecs_gc_alloc_array(cortecs_type_arg(cortecs_lsp_any), size);

    // read all fields into the array
    uint32_t index = 0;
    current = field->child;
    while (current != NULL) {
        elements->elements[index] = accept_any(current);
        current = current->next;
    }

    return elements;
}

static cortecs_lsp_any accept_any(const cJSON *field) {
    if (is_array(field)) {
        return (cortecs_lsp_any){
            .tag = CORTECS_LSP_ANY_ARRAY,
            .value.array = accept_array(field),
        };
    }

    if (is_object(field)) {
        return (cortecs_lsp_any){
            .tag = CORTECS_LSP_ANY_OBJECT,
            .value.object = accept_object(field),
        };
    }

    if (is_string(field)) {
        return (cortecs_lsp_any){
            .tag = CORTECS_LSP_ANY_STRING,
            .value.string = accept_string(field),
        };
    }

    assert(false);
}

cortecs_lsp_parse_error_t parse_lsp_message(const cJSON *json, cortecs_lsp_message *message) {
    cortecs_lsp_parse_error_t error_message;

    const cJSON *jsonrpc_field;
    error_message = find_field(json, "jsonrpc", false, &jsonrpc_field);
    if (error_message.tag != CORTECS_LSP_PARSE_SUCCESS) {
        return error_message;
    }
    error_message = expect_string(jsonrpc_field);
    if (error_message.tag != CORTECS_LSP_PARSE_SUCCESS) {
        return error_message;
    }
    message->jsonrpc = accept_string(jsonrpc_field);

    return CORTECS_LSP_SUCCESS;
}

cortecs_lsp_parse_error_t parse_lsp_notification_message(const cJSON *json, cortecs_lsp_notification_message *message) {
    cortecs_lsp_parse_error_t error_message;

    error_message = parse_lsp_message(json, &message->super);
    if (error_message.tag != CORTECS_LSP_PARSE_SUCCESS) {
        return error_message;
    }

    const cJSON *method_field;
    error_message = find_field(json, "method", false, &method_field);
    if (error_message.tag != CORTECS_LSP_PARSE_SUCCESS) {
        return error_message;
    }
    error_message = expect_string(method_field);
    if (error_message.tag != CORTECS_LSP_PARSE_SUCCESS) {
        return error_message;
    }
    message->method = accept_string(method_field);

    const cJSON *params_field;
    error_message = find_field(json, "params", true, &params_field);
    if (error_message.tag == CORTECS_LSP_PARSE_SUCCESS_NOT_FOUND) {
        message->params.is_set = false;
        goto end_of_params;
    } else if (error_message.tag != CORTECS_LSP_PARSE_SUCCESS) {
        return error_message;
    }
    message->params.is_set = true;
    if (is_array(params_field)) {
        message->params.tag = CORTECS_LSP_NOTIFICATION_MESSAGE_PARAMS_ARRAY;
        message->params.value.array = accept_array(params_field);
        goto end_of_params;
    }
    if (is_object(params_field)) {
        message->params.tag = CORTECS_LSP_NOTIFICATION_MESSAGE_PARAMS_OBJECT;
        message->params.value.object = accept_object(params_field);
        goto end_of_params;
    }
    return incorrect_type_message(params_field, "array or object");
end_of_params:

    return CORTECS_LSP_SUCCESS;
}