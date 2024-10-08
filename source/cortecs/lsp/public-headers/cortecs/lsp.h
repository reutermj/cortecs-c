#ifndef CORTECS_LSP_LSP_H
#define CORTECS_LSP_LSP_H

#include <cortecs/array.h>
#include <cortecs/gc.h>
#include <cortecs/string.h>
#include <stdbool.h>

typedef struct {
    enum {
        CORTECS_LSP_PARSE_SUCCESS,
        CORTECS_LSP_PARSE_SUCCESS_NOT_FOUND,
        CORTECS_LSP_PARSE_MISSING_REQUIRED_FIELD,
        CORTECS_LSP_PARSE_TYPE_ERROR,
    } tag;
    cortecs_string message;
} cortecs_lsp_parse_error_t;

typedef struct cortecs_lsp_any cortecs_lsp_any;
extern cortecs_finalizer_declare(cortecs_lsp_any);
cortecs_array_forward_declare(cortecs_lsp_any);

typedef struct {
    cortecs_array(cortecs_string) field_names;
    cortecs_array(cortecs_lsp_any) field_values;
} cortecs_lsp_object;

struct cortecs_lsp_any {
    enum {
        CORTECS_LSP_ANY_STRING,
        CORTECS_LSP_ANY_INTEGER,
        CORTECS_LSP_ANY_UINTEGER,
        CORTECS_LSP_ANY_DECIMAL,
        CORTECS_LSP_ANY_BOOLEAN,
        CORTECS_LSP_ANY_OBJECT,
        CORTECS_LSP_ANY_ARRAY,
        CORTECS_LSP_ANY_NULL,
    } tag;

    union {
        cortecs_string string;
        int32_t integer;
        uint32_t uinteger;
        float decimal;
        bool boolean;
        cortecs_lsp_object object;
        cortecs_array(cortecs_lsp_any) array;
    } value;
};
cortecs_array_define(cortecs_lsp_any);

typedef struct {
    cortecs_string jsonrpc;
} cortecs_lsp_message;

typedef struct {
    cortecs_lsp_message super;
    /**
     * The method to be invoked.
     */
    cortecs_string method;

    /**
     * The notification's params.
     */
    struct {
        bool is_set;
        enum {
            CORTECS_LSP_NOTIFICATION_MESSAGE_PARAMS_ARRAY,
            CORTECS_LSP_NOTIFICATION_MESSAGE_PARAMS_OBJECT,
        } tag;
        union {
            cortecs_array(cortecs_lsp_any) array;
            cortecs_lsp_object object;
        } value;
    } params;
} cortecs_lsp_notification_message;

#endif