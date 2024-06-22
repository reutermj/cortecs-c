#include "lsp.h"

#include <cJSON.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static lsp_parse_error_t expect_string(cJSON *json, const char *field_name, string_t *out) {
    const cJSON *field = cJSON_GetObjectItemCaseSensitive(json, field_name);
    if (field == NULL) {
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

    if (!cJSON_IsString(field) || field->valuestring == NULL) {
        char *error_message = malloc(LSP_ERROR_MESSAGE_MAX_SIZE);
        snprintf(
            error_message,
            LSP_ERROR_MESSAGE_MAX_SIZE,
            "%s expected to be string, found %s",
            field_name,
            cJSON_Print(field)
        );

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

    error_message = expect_string(json, "jsonrpc", &message->jsonrpc);
    if (error_message.tag != LSP_PARSE_SUCCESS) {
        return error_message;
    }

    return (lsp_parse_error_t){
        .tag = LSP_PARSE_SUCCESS,
        .message = NULL,
    };
}