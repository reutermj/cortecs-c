load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "parser",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["./"],
    visibility = ["//cortecs:__subpackages__"],
    deps = [
        "//common",
        "//cortecs/common",
        "//cortecs/lexer",
        "@flecs",
    ],
)

cc_library(
    name = "private",
    hdrs = glob(["private/*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["./"],
    visibility = ["//cortecs/parser:__subpackages__"],
    deps = ["//cortecs/parser"],
)
