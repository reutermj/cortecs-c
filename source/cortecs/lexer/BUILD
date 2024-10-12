cc_library(
    name = "lexer",
    srcs = glob(["*.c"]),
    hdrs = glob(["public-headers/cortecs/*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["public-headers/"],
    visibility = ["//visibility:public"],
    deps = [
        "//source/cortecs/gc",
        "//source/cortecs/string",
        "@icu//icu4c/source/common:errorcode",
        "@icu//icu4c/source/common:headers",
        "@icu//icu4c/source/common:uchar",
        "@icu//icu4c/source/common:utext",
    ],
)
