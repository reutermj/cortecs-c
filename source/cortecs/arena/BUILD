cc_library(
    name = "arena",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]) + glob(["public-headers/cortecs/*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["public-headers/"],
    visibility = ["//visibility:public"],
    deps = [
        "//source/common",
        "//source/cortecs/array",
    ],
)
