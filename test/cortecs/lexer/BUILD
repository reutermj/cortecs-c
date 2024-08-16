cc_test(
    name = "lexer",
    size = "small",
    srcs = [
        "test_configs.c",
        "test_configs.h",
        "test_impls.c",
        "test_impls.h",
        "test_lexer.c",
    ],
    features = ["treat_warnings_as_errors"],
    deps = [
        "//source/common",
        "//source/cortecs/lexer",
        "@unity",
    ],
)

cc_test(
    name = "span",
    size = "small",
    srcs = ["test_span.c"],
    features = ["treat_warnings_as_errors"],
    deps = [
        "//source/cortecs/lexer",
        "@unity",
    ],
)
