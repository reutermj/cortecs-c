load("@rules_cc//cc:defs.bzl", "cc_library")

cc_library(
    name = "lexer",
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
        "//source/cortecs/common",
        "//third-party/icu4c/common:errorcode",
        "//third-party/icu4c/common:headers",
        "//third-party/icu4c/common:uchar",
        "//third-party/icu4c/common:utext",
    ],
)
