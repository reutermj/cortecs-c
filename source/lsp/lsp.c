#include "lsp.h"

#include <cJSON.h>
#include <common.h>
#include <persistent_string.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define LSP_SUCCESS               \
    (lsp_parse_error_t) {         \
        .tag = LSP_PARSE_SUCCESS, \
        .message = NULL,          \
    }

static lsp_parse_error_t accept_any(const cJSON *field, lsp_any *out);

static lsp_parse_error_t find_field(cJSON *json, const char *field_name, bool is_optional, const cJSON **out) {
    const cJSON *field = cJSON_GetObjectItemCaseSensitive(json, field_name);
    if (field == NULL) {
        if (is_optional) {
            return (lsp_parse_error_t){
                .tag = LSP_PARSE_SUCCESS_NOT_FOUND,
                .message = NULL,
            };
        }

        char *error_message = malloc(LSP_ERROR_MESSAGE_MAX_SIZE);
        snprintf(
            error_message,
            LSP_ERROR_MESSAGE_MAX_SIZE,
            "%s expected but not found",
            field_name
        );

        return (lsp_parse_error_t){
            .tag = LSP_PARSE_MISSING_REQUIRED_FIELD,
            .message = error_message,
        };
    }

    *out = field;

    return LSP_SUCCESS;
}

static lsp_parse_error_t accept_string(const cJSON *field, bool is_optional, string_t *out) {
    if (!cJSON_IsString(field) || field->valuestring == NULL) {
        if (is_optional) {
            return (lsp_parse_error_t){
                .tag = LSP_PARSE_SUCCESS_NOT_FOUND,
                .message = NULL,
            };
        }

        char *error_message = malloc(LSP_ERROR_MESSAGE_MAX_SIZE);
        char *field_str = cJSON_Print(field);
        snprintf(
            error_message,
            LSP_ERROR_MESSAGE_MAX_SIZE,
            "%s expected to be string, found %s",
            field->string,
            field_str
        );
        free(field_str);

        return (lsp_parse_error_t){
            .tag = LSP_PARSE_TYPE_ERROR,
            .message = error_message,
        };
    }

    uint8_t *name_string = (uint8_t *)field->valuestring;
    *out = (string_t){
        .content = name_string,
        .length = strlen((const char *)name_string),  // TODO probably need to fix this because these messages are user inputted
    };

    return LSP_SUCCESS;
}

static lsp_parse_error_t accept_object(const cJSON *field, bool is_optional, lsp_object *out) {
    if (!cJSON_IsObject(field) || field->child == NULL) {
        if (is_optional) {
            return (lsp_parse_error_t){
                .tag = LSP_PARSE_SUCCESS_NOT_FOUND,
                .message = NULL,
            };
        }

        char *error_message = malloc(LSP_ERROR_MESSAGE_MAX_SIZE);
        char *field_str = cJSON_Print(field);
        snprintf(
            error_message,
            LSP_ERROR_MESSAGE_MAX_SIZE,
            "%s expected to be object, found %s",
            field->string,
            field_str
        );
        free(field_str);

        return (lsp_parse_error_t){
            .tag = LSP_PARSE_TYPE_ERROR,
            .message = error_message,
        };
    }

    cJSON *first = field->child;
    cJSON *current = first;

    uint32_t length = 0;
    while (current != NULL) {
        length++;
        current = current->next;
    }

    string_t *names = malloc(sizeof(string_t) * length);
    lsp_any *values = malloc(sizeof(lsp_any) * length);

    uint32_t index = 0;
    current = first;
    while (current != NULL) {
        names[index] = (string_t){
            .length = strlen(current->string),  // TODO probably need to fix this because these messages are user inputted
            .content = (uint8_t *)current->string,
        };

        lsp_parse_error_t error_message = accept_any(current, &values[index]);
        if (error_message.tag != LSP_PARSE_SUCCESS) {
            free(names);
            free(values);
            return error_message;
        }

        current = current->next;
    }

    *out = (lsp_object){
        .num_fields = length,
        .field_names = names,
        .field_values = values,
    };

    return LSP_SUCCESS;
}

static lsp_parse_error_t accept_array(const cJSON *field, bool is_optional, lsp_array *out) {
    if (!cJSON_IsArray(field) || field->child == NULL) {
        if (is_optional) {
            return (lsp_parse_error_t){
                .tag = LSP_PARSE_SUCCESS_NOT_FOUND,
                .message = NULL,
            };
        }

        char *error_message = malloc(LSP_ERROR_MESSAGE_MAX_SIZE);
        char *field_str = cJSON_Print(field);
        snprintf(
            error_message,
            LSP_ERROR_MESSAGE_MAX_SIZE,
            "%s expected to be array, found %s",
            field->string,
            field_str
        );
        free(field_str);

        return (lsp_parse_error_t){
            .tag = LSP_PARSE_TYPE_ERROR,
            .message = error_message,
        };
    }

    cJSON *first = field->child;
    cJSON *current = first;

    uint32_t length = 0;
    while (current != NULL) {
        length++;
        current = current->next;
    }

    lsp_any *elements = malloc(sizeof(lsp_any) * length);

    uint32_t index = 0;
    current = first;
    while (current != NULL) {
        lsp_parse_error_t error_message = accept_any(current, &elements[index]);
        if (error_message.tag != LSP_PARSE_SUCCESS) {
            free(elements);
            return error_message;
        }

        current = current->next;
    }

    *out = (lsp_array){
        .length = length,
        .content = elements,
    };

    return LSP_SUCCESS;
}

static lsp_parse_error_t accept_any(const cJSON *field, lsp_any *out) {
    if (cJSON_IsArray(field)) {
        out->tag = LSP_ANY_ARRAY;
        return accept_array(field, false, &out->value.array);
    }

    if (cJSON_IsObject(field)) {
        out->tag = LSP_ANY_OBJECT;
        return accept_object(field, false, &out->value.object);
    }

    if (cJSON_IsString(field)) {
        out->tag = LSP_ANY_STRING;
        return accept_string(field, false, &out->value.string);
    }

    // TODO

    return LSP_SUCCESS;
}

lsp_parse_error_t parse_lsp_message(cJSON *json, lsp_message *message) {
    lsp_parse_error_t error_message;
    const cJSON *jsonrpc_field;
    error_message = find_field(json, "jsonrpc", false, &jsonrpc_field);
    if (error_message.tag != LSP_PARSE_SUCCESS) {
        return error_message;
    }

    error_message = accept_string(json, false, &message->jsonrpc);
    if (error_message.tag != LSP_PARSE_SUCCESS) {
        return error_message;
    }

    return (lsp_parse_error_t){
        .tag = LSP_PARSE_SUCCESS,
        .message = NULL,
    };
}

lsp_parse_error_t parse_lsp_notification_message(cJSON *json, lsp_notification_message *message) {
    lsp_parse_error_t error_message;

    parse_lsp_message(json, &message->super);

    const cJSON *method_field;
    error_message = find_field(json, "method", false, &method_field);
    if (error_message.tag != LSP_PARSE_SUCCESS) {
        return error_message;
    }
    error_message = accept_string(method_field, false, &message->method);
    if (error_message.tag != LSP_PARSE_SUCCESS) {
        return error_message;
    }

    const cJSON *params_field;
    error_message = find_field(json, "params", true, &params_field);
    if (error_message.tag == LSP_PARSE_SUCCESS_NOT_FOUND) {
        message->params.is_set = false;
        goto end_of_params;
    } else if (error_message.tag != LSP_PARSE_SUCCESS) {
        return error_message;
    }
    message->params.is_set = true;
    error_message = accept_array(params_field, false, &message->params.value.array);
    if (error_message.tag == LSP_PARSE_SUCCESS) {
        message->params.tag = LSP_NOTIFICATION_MESSAGE_PARAMS_ARRAY;
        goto end_of_params;
    }
    error_message = accept_object(params_field, false, &message->params.value.object);
    if (error_message.tag == LSP_PARSE_SUCCESS) {
        message->params.tag = LSP_NOTIFICATION_MESSAGE_PARAMS_OBJECT;
        goto end_of_params;
    }
    return error_message;
end_of_params:

    return (lsp_parse_error_t){
        .tag = LSP_PARSE_SUCCESS,
        .message = NULL,
    };
}