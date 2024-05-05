load("@rules_cc//cc:defs.bzl", "cc_test")

cc_test(
    name = "flecs-integration",
    size = "small",
    srcs = glob(["*.c"]),
    features = ["treat_warnings_as_errors"],
    deps = [
        "@flecs",
        "@unity",
    ],
)
