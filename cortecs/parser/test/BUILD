load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "test",
    size = "small",
    srcs = glob(["*.c"]),
    features = ["treat_warnings_as_errors"],
    deps = [
        "//cortecs/parser:private",
        "@unity",
    ],
)
