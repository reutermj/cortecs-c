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
        "//source/cortecs/gc",
        "@icu//icu4c/source/common:errorcode",
        "@icu//icu4c/source/common:headers",
        "@icu//icu4c/source/common:uchar",
        "@icu//icu4c/source/common:utext",
    ],
)
