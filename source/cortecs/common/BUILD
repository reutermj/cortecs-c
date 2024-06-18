load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "common",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["./"],
    visibility = [
        "//source/cortecs:__subpackages__",
        "//test/cortecs:__subpackages__",
    ],
    deps = [
        "//source/common",
        "@flecs",
    ],
)
