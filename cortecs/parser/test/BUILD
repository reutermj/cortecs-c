load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "test",
    size = "small",
    srcs = glob(["*.c"]),
    deps = [
        "//cortecs/parser",
        "@unity",
        "@sds",
    ],
)
