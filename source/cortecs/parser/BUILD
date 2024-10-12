cc_library(
    name = "parser",
    srcs = glob(["*.c"]),
    hdrs = glob(["public-headers/cortecs/*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["public-headers/"],
    visibility = ["//visibility:public"],
    deps = [
        "//source/cortecs/lexer",
        "@flecs",
    ],
)
