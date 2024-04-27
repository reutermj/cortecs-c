load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "lexer",
    srcs = glob(["*.c"]),
    hdrs = glob(["*.h"]),
    features = ["treat_warnings_as_errors"],
    includes = ["./"],
    visibility = ["//cortecs:__subpackages__"],
)
