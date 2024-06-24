#include "lsp.h"

#include <cJSON.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

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

    return (lsp_parse_error_t){
        .tag = LSP_PARSE_SUCCESS,
        .message = NULL,
    };
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
        .length = strlen((const char *)name_string),
    };

    return (lsp_parse_error_t){
        .tag = LSP_PARSE_SUCCESS,
        .message = NULL,
    };
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