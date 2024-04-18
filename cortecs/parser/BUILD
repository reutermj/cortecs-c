load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "parser",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    includes = ["./"],
    deps = [
        "//cortecs/ast",
        "@flecs",
        "@sds",
    ],
)
