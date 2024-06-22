#ifndef LSP_LSP_H
#define LSP_LSP_H

#include <persistent_string.h>
#include <stdbool.h>

#define LSP_ERROR_MESSAGE_MAX_SIZE (sizeof(char) * 256)

typedef struct {
    enum {
        LSP_PARSE_SUCCESS,
        LSP_PARSE_MISSING_REQUIRED_FIELD,
        LSP_PARSE_TYPE_ERROR,
    } tag;
    char *message;
} lsp_parse_error_t;

typedef struct {
    enum {
        LSP_ANY_STRING,
        LSP_ANY_INTEGER,
        LSP_ANY_UINTEGER,
        LSP_ANY_DECIMAL,
        LSP_ANY_BOOLEAN,
        LSP_ANY_NULL,
    } tag;

    union {
        string_t string;
        int32_t integer;
        uint32_t uinteger;
        float decimal;
        bool boolean;
    } value;
} lsp_any;

typedef struct {
    uint32_t length;
    lsp_any *content;
} lsp_array;

typedef struct {
    string_t jsonrpc;
} lsp_message;

typedef struct {
    lsp_message super;
    /**
     * The request id.
     */
    struct {
        enum {
            LSP_REQUEST_MESSAGE_INTEGER,
            LSP_REQUEST_MESSAGE_STRING,
        } tag;
        union {
            int32_t integer;
            string_t string;
        } value;
    } id;

    /**
     * The method to be invoked.
     */
    string_t method;

    /**
     * The method's params.
     */
    struct {
        bool is_set;
        enum {
            LSP_REQUEST_MESSAGE_ARRAY,
            LSP_REQUEST_MESSAGE_OBJECT,
        } tag;
        union {
            lsp_array array;
            lsp_any object;
        } value;
    } params;
} lsp_request_message;

#endif