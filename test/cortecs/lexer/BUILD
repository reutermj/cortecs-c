load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "lexer",
    size = "small",
    srcs = glob([
        "*.c",
        "*.h",
    ]),
    features = ["treat_warnings_as_errors"],
    deps = [
        "//source/common",
        "//source/cortecs/lexer",
        "@unity",
    ],
)
