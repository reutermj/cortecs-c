cc_library(
    name = "lsp",
    srcs = glob(["*.c"]),
    hdrs = glob(["public-headers/cortecs/*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["public-headers/"],
    visibility = ["//visibility:public"],
    deps = [
        "//source/cortecs/array",
        "//source/cortecs/finalizer",
        "//source/cortecs/gc",
        "//source/cortecs/string",
        "@cJSON",
    ],
)
