load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "gc",
    size = "small",
    srcs = glob([
        "*.c",
    ]),
    features = ["treat_warnings_as_errors"],
    deps = [
        "//source/cortecs/gc",
        "@unity",
    ],
)
