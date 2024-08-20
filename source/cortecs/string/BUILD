cc_library(
    name = "string",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]) + glob(["public-headers/cortecs/*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["public-headers/"],
    visibility = ["//visibility:public"],
    deps = [
        "//source/cortecs/array",
        "//source/cortecs/gc",
    ],
)
