#ifndef CORTECS_LSP_LSP_H
#define CORTECS_LSP_LSP_H

#include <cortecs/string.h>
#include <stdbool.h>

#define LSP_ERROR_MESSAGE_MAX_SIZE (sizeof(char) * 256)

typedef struct {
    enum {
        LSP_PARSE_SUCCESS,
        LSP_PARSE_SUCCESS_NOT_FOUND,
        LSP_PARSE_MISSING_REQUIRED_FIELD,
        LSP_PARSE_TYPE_ERROR,
    } tag;
    char *message;
} lsp_parse_error_t;

typedef struct lsp_any lsp_any;

typedef struct {
    uint32_t num_fields;
    string_t *field_names;
    lsp_any *field_values;
} lsp_object;

typedef struct {
    uint32_t length;
    lsp_any *content;
} lsp_array;

struct lsp_any {
    enum {
        LSP_ANY_STRING,
        LSP_ANY_INTEGER,
        LSP_ANY_UINTEGER,
        LSP_ANY_DECIMAL,
        LSP_ANY_BOOLEAN,
        LSP_ANY_OBJECT,
        LSP_ANY_ARRAY,
        LSP_ANY_NULL,
    } tag;

    union {
        string_t string;
        int32_t integer;
        uint32_t uinteger;
        float decimal;
        bool boolean;
        lsp_object object;
        lsp_array array;
    } value;
};

typedef struct {
    string_t jsonrpc;
} lsp_message;

typedef struct {
    lsp_message super;
    /**
     * The method to be invoked.
     */
    string_t method;

    /**
     * The notification's params.
     */
    struct {
        bool is_set;
        enum {
            LSP_NOTIFICATION_MESSAGE_PARAMS_ARRAY,
            LSP_NOTIFICATION_MESSAGE_PARAMS_OBJECT,
        } tag;
        union {
            lsp_array array;
            lsp_object object;
        } value;
    } params;
} lsp_notification_message;

#endif